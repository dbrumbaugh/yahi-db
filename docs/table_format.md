# Ideas for Table Storage Format

We need to track a few pieces of metadata about each table on disk. These are,
as of now,
* the schema
  * Each field's length and data type (and name?)
* the number of records
* the record length

Perhaps a format like (in binary, no newlines),

<db_name>.<tbl_name> (as normal, null terminated, strings)
<record_count (as a long)>
<record_length (as an int)>
(schema data)
<name (as null terminated string)>
<type (as int)
<length (as int)>

<name (as null terminated string)>
<type (as int)
<length (as int)>
.
.
.
<null><null><null>
<record data>
