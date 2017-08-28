/* "matrix" type of rotate and scale */
CREATE TYPE matrix_rotate_scale AS
   ( m11 double precision,
     m12 double precision,  
     m21 double precision,
     m22 double precision );

/* "matrix" type of shift */
CREATE TYPE matrix_shift AS
   ( s1 double precision,
     s2 double precision );

/* table of elements */
CREATE TABLE transform_matrix
(
   facility_id integer NOT NULL,
   rotate_scale matrix_rotate_scale NOT NULL, 
   shift matrix_shift NOT NULL, 
   FOREIGN KEY (facility_id) REFERENCES areas (id) ON UPDATE NO ACTION ON DELETE CASCADE
);
