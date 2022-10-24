#! /bin/sh

# select * from Typecast_table_1;
# select * from Typecast_table_1 WHERE age>'11';

# select * from Typecast_table_2;
# select * from Typecast_table_2 WHERE age<=10;

# select * from Typecast_table_3;
# select * from Typecast_table_3 WHERE id<'1.5a';
# select * from Typecast_table_3 WHERE id>='3a';

build/bin/obclient << EOF

select max(aa), min(aa) from s;

exit
EOF
# CREATE TABLE Typecast_table_4(id int, name char, age float);
# INSERT INTO Typecast_table_4 VALUES (1,1,1);
# SELECT * FROM Typecast_table_4;
# SELECT * FROM Typecast_table_4 WHERE 'a'<1;
# SELECT * FROM Typecast_table_4 WHERE '1a'>0;
# SELECT * FROM Typecast_table_4 WHERE '1a'>0.5;

# CREATE TABLE Typecast_table_3(id int, name char, age float);
# INSERT INTO Typecast_table_3 VALUES (1,1,1);
# INSERT INTO Typecast_table_3 VALUES (2,2,2);
# INSERT INTO Typecast_table_3 VALUES (3,3,3);
# select * from Typecast_table_3;
# select * from Typecast_table_3 WHERE id<'1.5a';
# select * from Typecast_table_3 WHERE id>='3a';

# CREATE TABLE Typecast_table_2(id int, name char, age float);
# INSERT INTO Typecast_table_2 VALUES (0.4,1.5,9.5);
# INSERT INTO Typecast_table_2 VALUES (1.0,2.50,9.5);
# INSERT INTO Typecast_table_2 VALUES (1.9,'3.5',11.5);
# select * from Typecast_table_2;
# select * from Typecast_table_2 WHERE age<=10;

# CREATE TABLE Typecast_table_1(id int, name char, age float);
# INSERT INTO Typecast_table_1 VALUES ('cc','c','ccc');
# INSERT INTO Typecast_table_1 VALUES ('1a','a','11.5');
# INSERT INTO Typecast_table_1 VALUES ('2','b','11.5b');
# INSERT INTO Typecast_table_1 VALUES ('2','b','11.5b');
# select * from Typecast_table_1;
# select * from Typecast_table_1 WHERE age>'11';


# CREATE TABLE Select_tables_1(id int, age int, u_name char);
# CREATE TABLE Select_tables_2(id int, age int, u_name char);
# CREATE TABLE Select_tables_3(id int, res int, u_name char);
# CREATE TABLE Select_tables_4(id int, age int, u_name char);
# CREATE TABLE Select_tables_5(id int, res int, u_name char);
# INSERT INTO Select_tables_1 VALUES (1,18,'a');
# INSERT INTO Select_tables_1 VALUES (2,15,'b');
# INSERT INTO Select_tables_2 VALUES (1,20,'a');
# INSERT INTO Select_tables_2 VALUES (2,21,'c');
# INSERT INTO Select_tables_3 VALUES (1,35,'a');
# INSERT INTO Select_tables_3 VALUES (2,37,'a');
# INSERT INTO Select_tables_4 VALUES (1, 2, 'a');
# INSERT INTO Select_tables_4 VALUES (1, 3, 'b');
# INSERT INTO Select_tables_4 VALUES (2, 2, 'c');
# INSERT INTO Select_tables_4 VALUES (2, 4, 'd');
# INSERT INTO Select_tables_5 VALUES (1, 10, 'g');
# INSERT INTO Select_tables_5 VALUES (1, 11, 'f');
# INSERT INTO Select_tables_5 VALUES (2, 12, 'c');
# CREATE TABLE Select_tables_7(id int, col1 char, col2 int);
# CREATE TABLE Select_tables_8(id int, col1 int, col2 float);
# INSERT INTO Select_tables_7 VALUES (1,'a',9);
# INSERT INTO Select_tables_7 VALUES (2,'b',10);
# INSERT INTO Select_tables_7 VALUES (3,'c',11);
# INSERT INTO Select_tables_8 VALUES (1,0,9.5);
# INSERT INTO Select_tables_8 VALUES (2,1,10.5);
# INSERT INTO Select_tables_8 VALUES (3,2,11.5);
# SELECT Select_tables_7.id, Select_tables_7.col1, Select_tables_7.col2, Select_tables_8.id, Select_tables_8.col1, Select_tables_8.col2 FROM Select_tables_7, Select_tables_8 where Select_tables_7.col1 <> Select_tables_8.col1;