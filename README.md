# b-RDBMS

A layered relational database management system (RDBMS) implemented in C. The system is built from the ground up with four distinct abstraction layers, each providing progressively higher-level database functionality.

## Architecture

The system follows a classic layered DBMS architecture:

```
+---------------------------+
|    VW_Level (Views)       |   Materialized view creation/destruction
+---------------------------+
|    DM_Level (Data Mgmt)   |   SQL-like operations: SELECT, JOIN, INSERT, DELETE
+---------------------------+
|    AM_Level (Access Mgmt) |   Hash-based indexing (extendible hashing)
+---------------------------+
|    BF_Level (Block/File)  |   Block-level I/O with LRU buffer management
+---------------------------+
```

### BF_Level - Block File Layer

The lowest layer, providing block-level file I/O with a buffer pool manager:

- **Buffer pool**: 20 block slots of 1024 bytes each, managed with LRU replacement
- **Block pinning**: Pin/unpin semantics with dirty-bit tracking for write-back
- **Bitmap management**: Tracks valid/free blocks within files
- **File operations**: Create, destroy, open, close files (up to 25 open simultaneously)

Key files: `BF_Lib.h`, `File_Functions.c`, `Block_Functions.c`, `Our_Functions.c`

### AM_Level - Access Method Layer

Implements hash-based indexing (extendible hashing) on top of the BF layer:

- **Extendible hashing**: Dynamic hash indexes with configurable depth (up to 8)
- **Index management**: Create, open, close, and destroy indexes on relation attributes
- **Index scans**: Supports equality, less-than, greater-than, and range comparisons
- **Data types**: Supports integer (`'i'`), float (`'f'`), and character (`'c'`) attribute types
- Up to 25 open indexes and 20 concurrent scans

Key files: `AM_Lib.h`, `AM_Lib.c`

### HF_Level - Heap File Layer

Provides record-level access within heap files (included as a library in AM_Level):

- **Record operations**: Insert, delete, and retrieve records by ID
- **Sequential scans**: Full table scans with optional filter conditions
- **Comparison operators**: `=`, `<`, `>`, `<=`, `>=`, `!=`

Key file: `HF_Lib.h`

### DM_Level - Data Manipulation Layer

The highest operational layer, implementing relational algebra operations with an SQL-like parser:

- **SQL-like parser**: Uses Flex/Bison (`lex.yy.c`, `parser3.tab.c`) to parse commands
- **System catalogs**: Maintains `RELCAT` (relation catalog), `ATTRCAT` (attribute catalog), `VIEWCAT`, and `VIEWATTRCAT`
- **Supported operations**:
  - `CREATE` - Create new relations with typed attributes
  - `DESTROY` - Drop relations and their indexes
  - `BUILDINDEX` - Create indexes on attributes
  - `SELECT` - Projection and selection (with optional `WHERE` clause)
  - `SELECT INTO` - Selection with output to a new relation
  - `JOIN` - Equi-join between two relations (index-aware optimization)
  - `INSERT` - Insert records (single or bulk from another relation)
  - `DELETE` - Delete records (all, conditional, or set subtraction)
  - `QUIT` - Exit the system

Key files: `main.c`, `DM_Join.c`, `parser3.h`, `parser3.tab.c`

### VW_Level - View Layer

Supports materialized views built on top of select and join operations:

- **View creation**: `VW_CreateSelect` and `VW_CreateJoin`
- **View destruction**: With dependency checking (prevents destroying views used by other views)

Key file: `views.c`

## Sample Data

The AM_Level includes sample data files for an athletics database:

- **ATHLETES** - Athlete records (id, surname, name)
- **EVENTS** - Event records (id, event name)
- **PARTICIPATIONS** - Participation records (athlete id, event id, date)

## Example Script

```sql
CREATE STUDENTS (studId='i', studName='c50', studYear='i');
CREATE GRADES (studId='i', studGrade='f');

BUILDINDEX STUDENTS(studId);
BUILDINDEX GRADES(studId);

SELECT (STUDENTS.studName, STUDENTS.studYear);
SELECT (STUDENTS.studName, STUDENTS.studYear) WHERE STUDENTS.studYear>4;
SELECT (STUDENTS.studName, GRADES.studGrade) WHERE STUDENTS.studId=GRADES.studId;

INSERT STUDENTS (studName="Antonis", studYear=5, studId=1023);

DELETE STUDENTS WHERE studYear>4;

DESTROY STUDENTS;
DESTROY GRADES;

QUIT;
```

## Building

The project is written in C and requires:

- A C compiler (e.g., `gcc`)
- Flex and Bison (for the DM_Level parser)

Each layer can be compiled independently. For example, to build the DM_Level:

```bash
cd DM_Level
gcc -o minirel main.c DM_Join.c lex.yy.c parser3.tab.c -L../AM_Level -L../BF_Level
```

For the BF_Level tests:

```bash
cd BF_Level
gcc -o bf1 main1.c File_Functions.c Block_Functions.c Our_Functions.c Errors_Function.c
```

> **Note**: Pre-compiled `.o` object files are included in some directories. Exact build commands may vary depending on your environment and linker configuration.

## Technical Details

- **Block size**: 1024 bytes
- **Buffer pool size**: 20 blocks
- **Max open files**: 25
- **Max hash depth**: 8
- **Max concurrent scans**: 20
- **Attribute name length**: 40 characters
- **Supported types**: `'i'` (int), `'f'` (float), `'c'` (char string)
