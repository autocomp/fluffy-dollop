/* graph, nodes, edges */
CREATE TABLE graphs
(
   id integer NOT NULL, 
   type character varying(50) NOT NULL,
   PRIMARY KEY (id), 
   FOREIGN KEY (id) REFERENCES entitys (id) ON UPDATE NO ACTION ON DELETE CASCADE
);

/* relation of edges - two nodes */
CREATE TABLE graph_edges
(
   id integer NOT NULL, 
   first_node integer NOT NULL, 
   second_node integer NOT NULL,
   PRIMARY KEY (id), 
   FOREIGN KEY (first_node) REFERENCES graphs (id) ON UPDATE NO ACTION ON DELETE CASCADE,
   FOREIGN KEY (second_node) REFERENCES graphs (id) ON UPDATE NO ACTION ON DELETE CASCADE
);