#ifndef __MLFW__MAT__DOUBLE__
#define __MLFW__MAT__DOUBLE__

struct __mlfw_mat_double;
typedef struct __mlfw_mat_double mlfw_mat_double;
mlfw_mat_double * mlfw_mat_double_create_new(dimension_t rows,dimension_t columns);
void mlfw_mat_double_destroy(mlfw_mat_double *matrix);
mlfw_mat_double * mlfw_mat_double_from_csv(const char *csv_file_name,mlfw_mat_double *matrix,mlfw_row_vec_string **header);
double mlfw_mat_double_get(mlfw_mat_double *matrix,index_t row,index_t column);
void mlfw_mat_double_set(mlfw_mat_double *matrix,index_t row,index_t column,double value);
void mlfw_mat_double_get_dimension(mlfw_mat_double *matrix,dimension_t *rows,dimension_t *columns);
void mlfw_mat_double_copy(mlfw_mat_double *target,mlfw_mat_double *source,index_t target_row_index,index_t target_column_index,index_t source_from_row_index,index_t source_from_column_index,index_t source_to_row_index,index_t source_to_column_index);
void mlfw_mat_double_fill(mlfw_mat_double *matrix,index_t from_row_index,index_t from_column_index,index_t upto_row_index,index_t upto_column_index,index_t what_to_fill);

mlfw_column_vector_double * mlfw_mat_double_create_column_vec(mlfw_mat_double * matrix,index_t index,mlfw_column_vector_double *vector);

mlfw_mat_double * mlfw_mat_double_shuffle(mlfw_mat_double *matrix,uint8_t shuffle_count,mlfw_mat_double *shuffled_matrix);
void mlfw_mat_double_to_csv(mlfw_mat_double *matrix, char *csv_file_name,mlfw_row_vec_string *header);
mlfw_mat_double * mlfw_mat_double_transpose(mlfw_mat_double *matrix,mlfw_mat_double *transposed_matrix);

double mlfw_mat_double_get_min(mlfw_mat_double *matrix,index_t start_r,index_t start_c,index_t end_r,index_t end_c);
double mlfw_mat_double_get_max(mlfw_mat_double *matrix,index_t start_r,index_t start_c,index_t end_r,index_t end_c);
#endif