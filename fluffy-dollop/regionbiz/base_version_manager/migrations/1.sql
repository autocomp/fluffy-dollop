/* copy marks to entitys */
INSERT INTO entitys( name, id, description, parent_id )
( SELECT name, id, description, parent_id FROM marks );

/* drop columns in marks */
ALTER TABLE marks
  DROP COLUMN parent_id;
ALTER TABLE marks
  DROP COLUMN name;
ALTER TABLE marks
  DROP COLUMN description;
ALTER TABLE marks
  ADD COLUMN type character varying(100);
ALTER TABLE marks
  ADD FOREIGN KEY (id) REFERENCES entitys (id) ON UPDATE NO ACTION ON DELETE CASCADE;

/* set default type of mark */
UPDATE marks SET type = 'defect' WHERE id > 0;
ALTER TABLE marks
  ALTER COLUMN type SET NOT NULL;

/* set specific type of marks */
UPDATE marks SET type = 'photo' WHERE id IN
  ( SELECT id FROM marks AS m JOIN metadata AS me ON (me.entity_id = m.id) 
  WHERE me.value = 'фотография' AND me.name = 'mark_type' );
UPDATE marks SET type = 'photo3d' WHERE id IN
  ( SELECT id FROM marks AS m JOIN metadata AS me ON (me.entity_id = m.id) 
  WHERE me.value = 'панорамная фотография' AND me.name = 'mark_type' );

/* add reference in metadata */
DELETE FROM metadata WHERE entity_id NOT IN ( SELECT id FROM entitys );
ALTER TABLE public.metadata
  ADD FOREIGN KEY (entity_id) REFERENCES public.entitys (id) ON UPDATE NO ACTION ON DELETE CASCADE;
