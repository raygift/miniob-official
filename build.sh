cd src/observer/sql/parser

flex --header-file=lex.yy.h lex_sql.l
bison -d -b yacc_sql yacc_sql.y

cd ../../../..

mkdir -p build &&\
cd build &&\
cmake ..\
    -DDEBUG=ON\
    -DCMAKE_C_COMPILER=/usr/bin/gcc\
    -DCMAKE_CXX_COMPILER=/usr/bin/g++ &&\
make -j4

