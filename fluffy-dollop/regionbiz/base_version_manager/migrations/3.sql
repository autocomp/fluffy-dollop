/* add rows for files and plans */
ALTER TABLE public.files
  ADD COLUMN name character varying(100);
ALTER TABLE public.plans
  ADD COLUMN opacity real NOT NULL DEFAULT 1;