hostname = '217.148.208.59'
#hostname = 'localhost'
username = 'vira'
password = 'zaebobr'
database = 'vira'

directory = "./migrations"

#create version table
def createVersionTable( conn ) :
    with conn.cursor() as cur:
	print "Start to create version table"
	try:
	    cur.execute( "CREATE TABLE versions \
		          ( \
		            version integer NOT NULL, \
		            doing_time timestamp(6) with time zone NOT NULL, \
		            CONSTRAINT versions_pkey PRIMARY KEY (version) \
		          )" )
	    conn.commit()
	    print "Table was created"
	except:
    	    print "Can't create version table"
    	    exit( 3 )

#check version of database
def checkVersion( conn, use_force ) :
    need_create = 0
    with conn.cursor() as cur:
	#res = cur.execute( "SELECT oid FROM ( SELECT to_regclass('public.versions') as oid ) \
	#			       as oid_sel where oid is not null;" )
	if 1:#cur.rowcount > 0:
	    cur.execute( "SELECT count(version), max(version) from versions" )
	    count, max = cur.fetchone()
	    if count > 0:
		return count, max
	    else:
		return 0, 0
	else:
	    print "Table of version don't exists"
	    if use_force:
		need_create = 1
	    else:
		exit( 2 )

    if need_create == 1:
        createVersionTable( conn )
    return 0, 0

# read args
def readArgs() :
    import sys
    args = sys.argv
    if len(args) != 2:
	print "Use 1st arg - [force,soft]"
	exit( 1 )
    if args[1] == "force":
	return 1
    else:
	return 0

def getAllFiles() :
    import os
    return os.listdir( directory )

def processFiles( files, version, conn ):
    import os
    number = {}
    for path in files:
	# get basename and convert to number of version
	number[ int( os.path.basename( path ).split( "." )[ 0 ] ) ] \
	    = directory + "/" + path
    for num in sorted( number ):
	if num > version:
	    sql = number[ num ]
	    print "Use file", sql
	    with open( sql ) as sql_f:
		sql_text = sql_f.read()
		with conn.cursor() as cur:
		    cur.execute( sql_text )
		    print "In process was", cur.rowcount, "rows"
		    cur.execute( "INSERT INTO versions( version, doing_time ) \
				  VALUES (" + str(num) + ", current_timestamp);" )
		    conn.commit()

# main code
print "Start version processing..."

use_force = readArgs()
print "Use force mode -", use_force

import psycopg2 as pg
conn = pg.connect( host=hostname, user=username, password=password, dbname=database )

count, version = checkVersion( conn, use_force )
print "Current version:", version, "of (", count, ")"
if count == 0:
    version = 0

files = getAllFiles()
print "We have", len(files), "sql files"

processFiles( files, version, conn )

print "Finish version processing"
conn.close()