#!/usr/bin/env python3

import pathlib
import subprocess
import sys
import tempfile


def sql(binary, database, statement):
    result = subprocess.run(
        [binary, "-bail", str(database), statement],
        text=True, capture_output=True, timeout=30,
    )
    if result.returncode:
        raise AssertionError(f"{binary}: {statement}\n{result.stderr}")
    return result.stdout.strip()


def check(actual, expected):
    if actual != expected:
        raise AssertionError(f"expected {expected!r}, got {actual!r}")


def roundtrip(writer, reader, database):
    sql(writer, database, """
        CREATE TABLE w(a INTEGER,b TEXT,c TEXT,PRIMARY KEY(a,b DESC)) WITHOUT ROWID;
        INSERT INTO w VALUES(1,'a','old'),(2,'b','old');
        CREATE TABLE t(id INTEGER PRIMARY KEY,a INTEGER,b TEXT);
        CREATE INDEX ti ON t(a,b DESC);
        INSERT INTO t VALUES(1,1,'a'),(2,2,'b');
        CREATE TABLE r(a REAL,b BLOB,c TEXT,PRIMARY KEY(a,b DESC)) WITHOUT ROWID;
        INSERT INTO r VALUES(1.5,x'00','old'),(2.5,x'ff','old');
        CREATE TABLE implicit(k TEXT PRIMARY KEY DESC,a INTEGER UNIQUE);
        INSERT INTO implicit VALUES('a',1),('b',2);
        SELECT dolt_commit('-Am','seed');
        SELECT dolt_tag('seed');
        SELECT dolt_branch('preserved');
    """)
    head = sql(writer, database, "SELECT dolt_hashof('HEAD');")
    check(sql(reader, database, """
        SELECT a,b,c FROM w WHERE a BETWEEN 1 AND 2 ORDER BY a;
        SELECT id FROM t INDEXED BY ti WHERE a BETWEEN 1 AND 2 ORDER BY a;
        SELECT a,hex(b) FROM r WHERE a=1.5;
        SELECT k FROM implicit WHERE a=1;
    """), "1|a|old\n2|b|old\n1\n2\n1.5|00\na")
    check(sql(reader, database, """
        UPDATE w SET c='new' WHERE a=1;
        SELECT changes();
        DELETE FROM w WHERE a=2;
        SELECT changes();
        UPDATE t SET b='c' WHERE id=1;
        DELETE FROM t WHERE id=2;
        UPDATE r SET c='new' WHERE a=1.5;
        SELECT changes();
        DELETE FROM r WHERE a=2.5;
        SELECT changes();
        UPDATE implicit SET a=3 WHERE k='a';
        DELETE FROM implicit WHERE k='b';
    """), "1\n1\n1\n1")
    duplicate = subprocess.run(
        [reader, "-bail", str(database), "INSERT INTO w VALUES(1,'a','duplicate');"],
        text=True, capture_output=True, timeout=30,
    )
    check(duplicate.returncode, 1)
    if "UNIQUE constraint failed" not in duplicate.stderr:
        raise AssertionError(duplicate.stderr)
    for binary in (reader, writer):
        check(sql(binary, database, """
            SELECT a,b,c FROM w;
            SELECT id,a,b FROM t INDEXED BY ti;
            SELECT a,hex(b),c FROM r;
            SELECT k,a FROM implicit;
            PRAGMA integrity_check;
        """), "1|a|new\n1|1|c\n1.5|00|new\na|3\nok")
        check(sql(binary, database, "SELECT dolt_hashof('HEAD');"), head)
        check(sql(binary, database, """
            SELECT a,b,c FROM dolt_at_w('seed') ORDER BY a;
            SELECT id,a,b FROM dolt_at_t('preserved') ORDER BY id;
        """), "1|a|old\n2|b|old\n1|1|a\n2|2|b")
    sql(reader, database, "SELECT dolt_commit('-Am','updated'); SELECT dolt_gc();")
    check(sql(writer, database, """
        SELECT a,b,c FROM w;
        SELECT a,b,c FROM dolt_at_w('seed') ORDER BY a;
        PRAGMA integrity_check;
    """), "1|a|new\n1|a|old\n2|b|old\nok")


if __name__ == "__main__":
    current, old = sys.argv[1:]
    with tempfile.TemporaryDirectory(prefix="doltlite-numeric-compat-") as tmp:
        for label, writer, reader in (
            ("old-to-current", old, current), ("current-to-old", current, old)
        ):
            roundtrip(writer, reader, pathlib.Path(tmp) / (label + ".db"))
            print(f"PASS: {label}: numeric/DESC reads, writes, history and GC")
