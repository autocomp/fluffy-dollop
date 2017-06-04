/* create table of groups */
CREATE TABLE groups
(
   group_id integer NOT NULL, 
   element_id integer NOT NULL, 
   PRIMARY KEY (group_id, element_id), 
   FOREIGN KEY (group_id) REFERENCES entitys (id) ON UPDATE NO ACTION ON DELETE CASCADE, 
   FOREIGN KEY (element_id) REFERENCES entitys (id) ON UPDATE NO ACTION ON DELETE CASCADE
)