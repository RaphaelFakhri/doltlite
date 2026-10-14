CREATE TABLE keyed(a INTEGER,b TEXT,c TEXT,PRIMARY KEY(a,b DESC)) WITHOUT ROWID;
INSERT INTO keyed VALUES(1,'a','base'),(2,'b','base');
CREATE TABLE indexed(id INTEGER PRIMARY KEY,a INTEGER,b TEXT);
CREATE INDEX numeric_desc ON indexed(a,b DESC);
INSERT INTO indexed VALUES(1,1,'a'),(2,2,'b');
CREATE TABLE extremes(a INTEGER,b TEXT,PRIMARY KEY(a,b DESC)) WITHOUT ROWID;
INSERT INTO extremes VALUES
  (-9223372036854775808,'a'),(-9223372036854775807,'a'),
  (-9007199254740993,'a'),(-9007199254740992,'a'),
  (9007199254740992,'a'),(9007199254740993,'a'),
  (9223372036854775806,'a'),(9223372036854775807,'a');
SELECT dolt_commit('-Am','numeric base');
SELECT dolt_tag('numeric-base');
SELECT dolt_checkout('-b','feature');
INSERT INTO keyed VALUES(3,'c','feature');
INSERT INTO indexed VALUES(3,3,'c');
SELECT dolt_commit('-Am','numeric feature');
SELECT dolt_checkout('main');
INSERT INTO keyed VALUES(4,'d','main');
INSERT INTO indexed VALUES(4,4,'d');
SELECT dolt_commit('-Am','numeric main');
SELECT dolt_checkout('-b','workspace');
INSERT INTO keyed VALUES(5,'e','staged');
INSERT INTO indexed VALUES(5,5,'e');
SELECT dolt_add('.');
INSERT INTO keyed VALUES(6,'f','working');
INSERT INTO indexed VALUES(6,6,'f');
SELECT dolt_checkout('main');
