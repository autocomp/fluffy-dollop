/* table of layers */
CREATE TABLE layers
(
   id integer, 
   priority integer NOT NULL, 
   name character varying(100) NOT NULL, 
   PRIMARY KEY (id), 
   UNIQUE (priority, id), 
   UNIQUE (name)
);

/* "variant" type of element */
CREATE TYPE layer_element AS
   ( name character varying(100),
     id integer );

/* table of elements */
CREATE TABLE layers_elements
(
   layer_id integer NOT NULL, 
   element layer_element, 
   type character varying(100) NOT NULL,
   FOREIGN KEY (layer_id) REFERENCES layers (id) ON UPDATE NO ACTION ON DELETE CASCADE
);
