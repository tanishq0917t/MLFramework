#include<mlfw_matrix.h>
#include<mlfw_vector.h>
#include<stdlib.h>
#include<stdio.h>
#include<time.h>
typedef struct __mlfw_mat_double
{
    double **data;
    dimension_t rows;
    dimension_t columns;
}mlfw_mat_double;
mlfw_mat_double * mlfw_mat_double_create_new(dimension_t rows,dimension_t columns)
{
    mlfw_mat_double *matrix;
    index_t r,k;
    if(rows<0 || columns<0) return NULL;
    matrix=(mlfw_mat_double *)malloc(sizeof(mlfw_mat_double));
    if(matrix==NULL) return NULL;
    matrix->data=(double **)malloc(sizeof(double *)*rows);
    if(matrix->data==NULL)
    {
        free(matrix);
        return NULL;
    }
    for(r=0;r<rows;r++)
    {
        matrix->data[r]=(double *)malloc(sizeof(double)*columns);
        if(matrix->data[r]==NULL)
        {
            for(k=0;k<r;k++) free(matrix->data[k]);
            free(matrix->data);
            free(matrix);
        }
    }
    matrix->rows=rows;
    matrix->columns=columns;
    return matrix;
}
void mlfw_mat_double_destroy(mlfw_mat_double *matrix)
{
    if(!matrix) return;
    index_t r;
    for(r=0;r<matrix->rows;r++) free(matrix->data[r]);
    free(matrix->data);
    free(matrix);
}
mlfw_mat_double * mlfw_mat_double_from_csv(const char *csv_file_name,mlfw_mat_double *matrix,mlfw_row_vec_string **header)
{
    char header_string[1025];
    index_t header_index;
    dimension_t rows=0,columns=0;
    index_t r=0,c=0;
    char double_string[1025];
    int index=0;
    double value;
    char m; //to reach each character of file
    FILE *file; //to open file
    if(csv_file_name==NULL) return NULL;
    file=fopen(csv_file_name,"r");
    if(!file) return NULL;
    //Logic to read first line starts here
    columns=0;
    while(1)
    {
        m=fgetc(file);
        if(feof(file)) break;
        if(m=='\r') continue;
        if(m==',') columns++;
        if(m=='\n') break;
    }
    columns++;
    //Logic to read first line ends here
    *header=mlfw_row_vec_string_create_new(columns);
    if(*header==NULL) return NULL;
    rewind(file);
    index=0;
    header_index=0;
    while(1) //logic to prepare header
    {
        m=fgetc(file);
        if(feof(file)) break;
        if(m=='\r') continue;
        if(m==',')
        {
            header_string[index]='\0';
            mlfw_row_vec_string_set(*header,header_index,header_string);
            header_index++;
            index=0;
            continue;
        }
        if(m=='\n')
        {
            header_string[index]='\0';
            mlfw_row_vec_string_set(*header,header_index,header_string);
            break;
        }
        header_string[index]=m;
        index++;
    }
    rows=0;
    columns=0;
    while(1) //counting number of rows and columns
    {
        m=fgetc(file);
        if(feof(file)) break;
        if(rows==0 && m==',') columns++;
        if(m=='\n') rows++;
    }
    columns++; // a,b,c means 2 commas and 2+1 columns
    if(matrix==NULL)
    {
        matrix=mlfw_mat_double_create_new(rows,columns);
        if(matrix==NULL)
        {
            fclose(file);
            mlfw_row_vec_string_destroy(*header);
            header=NULL;
            return NULL;
        }
    }
    else
    {
        if(matrix->rows!=rows || matrix->columns!=columns)
        {
            mlfw_row_vec_string_destroy(*header);
            header=NULL;
            return NULL;
        }
    }
    rewind(file); //move the internal pointer to first byte of file

    //logic to skip the header
    while(1)
    {
        m=fgetc(file);
        if(feof(file)) break;
        if(m=='\r') continue;
        if(m=='\n') break;
    }
    index=0;
    while(1) //populating matrix
    {
        m=fgetc(file);
        if(feof(file)) break;
        if(m=='\r') continue;
        if(m==',' || m=='\n')
        {
            double_string[index]='\0';
            value=strtod(double_string,NULL);
            index=0;
            matrix->data[r][c]=value;
            c++;
            if(c==matrix->columns)
            {
                r++;
                c=0;
            }
        }
        else
        {
            double_string[index]=m;
            index++;
        }
    }
    fclose(file);
    return matrix;
}

double mlfw_mat_double_get(mlfw_mat_double *matrix,index_t row,index_t column)
{
    double value=0.0;
    if(matrix==NULL) return value;
    if(row<0 && row>=matrix->rows) return value;
    if(column<0 && row>=matrix->columns) return value;
    return matrix->data[row][column];
}
void mlfw_mat_double_set(mlfw_mat_double *matrix,index_t row,index_t column,double value)
{
    if(matrix==NULL) return;
    if(row<0 && row>=matrix->rows) return;
    if(column<0 && row>=matrix->columns) return;
    matrix->data[row][column]=value;
}

void mlfw_mat_double_get_dimension(mlfw_mat_double *matrix,dimension_t *rows,dimension_t *columns)
{
    if(matrix==NULL) return;
    if(rows) *rows=matrix->rows;
    if(columns) *columns=matrix->columns;
}
void mlfw_mat_double_copy(mlfw_mat_double *target,mlfw_mat_double *source,index_t target_row_index,index_t target_column_index,index_t source_from_row_index,index_t source_from_column_index,index_t source_to_row_index,index_t source_to_column_index)
{
    if(target==NULL || source==NULL) return;
    if(target_row_index<0 || target_row_index>=target->rows) return;
    if(target_column_index<0 || target_column_index>=target->columns) return;
    if(source_from_row_index<0 || source_from_row_index>=source->rows) return;
    if(source_from_column_index<0 || source_from_column_index>=source->columns) return;
    if(source_from_row_index>source_to_row_index) return;
    if(source_from_column_index>source_to_column_index) return;
    index_t target_r,target_c;
    index_t source_r,source_c;
    target_r=target_row_index;
    source_r=source_from_row_index;
    while (source_r<=source_to_row_index) //big change
    {
        target_c=target_column_index;
        source_c=source_from_column_index;
        while (source_c<=source_to_column_index)
        {
            if(target_r>=0 && target_r<target->rows && target_c>=0 && target_c<target->columns)
            {
                target->data[target_r][target_c]=source->data[source_r][source_c];
            }
            source_c++;
            target_c++;
        }
        
        source_r++;
        target_r++;
    }
    
}
void mlfw_mat_double_fill(mlfw_mat_double *matrix,index_t from_row_index,index_t from_column_index,index_t upto_row_index,index_t upto_column_index,index_t what_to_fill)
{
    if(matrix==NULL) return;
    if(from_row_index>=matrix->rows) return;
    if(from_column_index>=matrix->columns) return;
    if(from_row_index<0) from_row_index=0;
    if(from_column_index<0) from_column_index=0;
    if(upto_row_index>=matrix->rows) upto_row_index=matrix->rows-1;
    if(upto_column_index>=matrix->columns) upto_column_index=matrix->columns-1;
    if(from_row_index>upto_row_index) return;
    if(from_column_index>upto_column_index) return;
    for(index_t r=from_row_index;r<=upto_row_index;r++)
    {
        for(index_t c=from_column_index;c<=upto_column_index;c++)
        {
            matrix->data[r][c]=what_to_fill;
        }
    }
}

mlfw_column_vector_double * mlfw_mat_double_create_column_vec(mlfw_mat_double * matrix,index_t index,mlfw_column_vector_double *vector)
{
    if(matrix==NULL) return NULL;
    if(index<0 || index>=matrix->columns) return NULL;
    if(vector==NULL)
    {
        vector=mlfw_column_vector_double_create_new(matrix->rows);
        if(vector==NULL) return NULL;
    }
    else
    {
        if(mlfw_column_vector_double_get_size(vector)!=matrix->rows) return NULL;
    }
    for(index_t r=0;r<matrix->rows;r++)
    {
        mlfw_column_vector_double_set(vector,r,matrix->data[r][index]);
    }
    return vector;
}

mlfw_mat_double * mlfw_mat_double_shuffle(mlfw_mat_double *matrix,uint8_t shuffle_count,mlfw_mat_double *shuffled_matrix)
{
    if(matrix==NULL) return NULL;
    if(shuffle_count==0) return NULL;
    if(shuffled_matrix==NULL)
    {
        shuffled_matrix=mlfw_mat_double_create_new(matrix->rows,matrix->columns);
        if(shuffled_matrix==NULL) return NULL;
    }
    else
    {
        if(shuffled_matrix->rows!=matrix->rows || shuffled_matrix->columns!=matrix->columns) return NULL;
    }
    mlfw_mat_double_copy(shuffled_matrix,matrix,0,0,0,0,matrix->rows-1,matrix->columns-1);

    // for(index_t r=0;r<shuffled_matrix->rows;r++)
    // {
    //     for(index_t c=0;c<shuffled_matrix->columns;c++)
    //     {
    //         printf("%lf ",shuffled_matrix->data[r][c]);
    //     }
    //     printf("\n");
    // }

    int r;
    index_t u,swp_idx,a,b;
    double temp;
    b=shuffled_matrix->rows-1;
    srand(time(NULL));
    for(uint8_t i=0;i<shuffle_count;i++)
    {
        for(u=0;u<=shuffled_matrix->rows-3;u++)
        {
            a=u+1;
            r=rand();
            swp_idx=(r%(b-a+1))+a;
            for(index_t c=0;c<shuffled_matrix->columns;c++)
            {
                temp=shuffled_matrix->data[u][c];
                shuffled_matrix->data[u][c]=shuffled_matrix->data[swp_idx][c];
                shuffled_matrix->data[swp_idx][c]=temp;
            }
        }
    }
    return shuffled_matrix;
}

void mlfw_mat_double_to_csv(mlfw_mat_double *matrix,char *csv_file_name,mlfw_row_vec_string *header)
{
    dimension_t header_size;
    index_t r,c;
    char separator,*ptr;
    FILE *file;
    if(matrix==NULL || csv_file_name==NULL) return;
    file=fopen(csv_file_name,"w");
    if(file==NULL) return;
    if(header==NULL) fputc('\n',file);
    else
    {
        header_size=mlfw_row_vec_string_get_size(header);
        for(index_t index=0;index<header_size;index++)
        {
            mlfw_row_vec_string_get(header,index,&ptr);
            if(ptr!=NULL)
            {
                fputs(ptr,file);
                free(ptr);
            }
            if(index<header_size-1) fputc(',',file);
            else fputc('\n',file);
        }
    }
    for(r=0;r<matrix->rows;r++)
    {
        for(c=0;c<matrix->columns;c++)
        {
            fprintf(file,"%lf",matrix->data[r][c]);
            if(c==matrix->columns-1) separator='\n';
            else separator=',';
            fputc(separator,file);
        }
    }
    fclose(file);
}

mlfw_mat_double * mlfw_mat_double_transpose(mlfw_mat_double *matrix,mlfw_mat_double *transposed_matrix)
{
    if(matrix==NULL) return NULL;
    if(transposed_matrix==NULL)
    {
        transposed_matrix=mlfw_mat_double_create_new(matrix->columns,matrix->rows);
        if(transposed_matrix==NULL) return NULL;
    }
    else
    {
        if(transposed_matrix->columns!=matrix->rows || transposed_matrix->rows!=matrix->columns) return NULL;
    }
    for(index_t r=0;r<matrix->rows;r++)
    {
        for(index_t c=0;c<matrix->columns;c++)
        {
            transposed_matrix->data[c][r]=matrix->data[r][c];
        }
    }
    return transposed_matrix;
}
double mlfw_mat_double_get_min(mlfw_mat_double *matrix,index_t start_r,index_t start_c,index_t end_r,index_t end_c)
{
    double minimum;
    index_t r, c;
    if (matrix==NULL) return 0.0;
    if(start_r<0) start_r=0;
    if(start_c<0) start_c=0;
    if (end_r>=matrix->rows) end_r=matrix->rows-1;
    if (end_c>=matrix->columns) end_c=matrix->columns-1;
    if (start_r>end_r) return 0.0;
    if (start_c>end_c) return 0.0;
    minimum=matrix->data[start_r][start_c];
    for(r=start_r;r<=end_r;r++)
    {
        for(c=start_c;c<=end_c;c++)
        {
            if(matrix->data[r][c]<minimum) minimum=matrix->data[r][c];
        }
    }
    return minimum;
}
double mlfw_mat_double_get_max(mlfw_mat_double *matrix,index_t start_r,index_t start_c,index_t end_r,index_t end_c)
{
    double maximum;
    index_t r, c;
    if (matrix==NULL) return 0.0;
    if(start_r<0) start_r=0;
    if(start_c<0) start_c=0;
    if (end_r>=matrix->rows) end_r=matrix->rows-1;
    if (end_c>=matrix->columns) end_c=matrix->columns-1;
    if (start_r>end_r) return 0.0;
    if (start_c>end_c) return 0.0;
    maximum=matrix->data[start_r][start_c];
    for(r=start_r;r<=end_r;r++)
    {
        for(c=start_c;c<=end_c;c++)
        {
            if(matrix->data[r][c]>maximum) maximum=matrix->data[r][c];
        }
    }
    return maximum;
}
