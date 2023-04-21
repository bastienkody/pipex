#! /usr/bin/zsh

ITA="\033[3m"
RED="\033[31m"
GREEN="\033[32m"
YEL="\033[33m"
END="\033[m"

#norminette
echo -n "Test norminette:"
norm=$(find . | egrep ".*(\.c|\.h)$" | norminette)
if [[ $(echo $norm | egrep -v "OK\!$") ]] ;
then
	echo "${RED} erreur de norme:${END}"
	echo $norm | egrep -v "OK\!$"
else
	echo "${GREEN} norm ok${END}"
fi

#makefile (dupliquer pour bonus?)
echo -n "Test makefile:"
make 1>/dev/null 2> stderrmake.txt
make > stdoutmakebis.txt
[[ -s stderrmake.txt ]] && echo "${RED} make a ecrit sur std err${END}" || echo -n "${GREEN} pas d'erreur make${END}" 
echo -n " -- "
cat stdoutmakebis.txt | egrep -viq "(nothin|already)" && echo "${RED}makefile relink?${END}" || echo "${GREEN}pas de relink${END}"
rm -rf stderrmake.txt stdoutmakebis.txt

#arg nb problem
echo "Test pb nombre arg (0, 1, 2 et 3):"
./pipex > no_arg.txt 2> no_arg_err.txt
./pipex Makefile > one_arg.txt 2> one_arg_err.txt
./pipex Makefile "echo yo" > two_arg.txt 2> two_arg_err.txt
./pipex Makefile "echo yo" "echo yi" > three_arg.txt 2> three_arg_err.txt
./pipex Makefile "echo yo" "echo yi" outfile > four_arg.txt 2> four_arg_err.txt
if [[ -s no_arg.txt && -s one_arg.txt && -s two_arg.txt && -s three_arg.txt ]] ;
then	echo "${YEL}pipex ecrit sur std out (fd 1)${END}"
fi
if [[ -s no_arg_err.txt || -s one_arg_err.txt || -s two_arg_err.txt || -s three_arg_err.txt ]] ;
then	echo "${GREEN}pipex ecrit sur std err (fd 2)${END}"
fi
if [[ $(cat no_arg.txt no_arg_err.txt one_arg.txt one_arg_err.txt two_arg.txt two_arg_err.txt three_arg.txt three_arg_err.txt | egrep "(yi|yo)") ]] ;
then	echo "${RED}pipex realise les commandes${END}"
fi
rm -rf *arg*.txt outfile

#basics tests
echo "${ITA}Test basics:${END}"

echo -n "Test 1 : ./pipex Makefile ls ls t1_output \t--> "
touch t1_output t1_expected #both created here bc of ls cmd
ls > t1_expected
./pipex "Makefile" "ls" "/usr/bin/ls" "t1_output" 
diff t1_expected t1_output && {echo "${GREEN}OK${END}" && rm t1_*} || echo "${RED}KO${END}"

echo -n "Test 2 : ./pipex Makefile cat cat t2_output\t\t--> "
touch t2_output
./pipex "Makefile" "cat" "cat" "t2_output" 
diff Makefile t2_output && {echo "${GREEN}OK${END}" && rm t2_*} || echo "${RED}KO${END}"

echo -n "Test 3 : ./pipex Makefile yes head t3_output \t\t--> "
touch t3_output t3_expected
yes | head > t3_expected
./pipex "Makefile" "yes" "head" "t3_output" 
diff t3_expected t3_output && {echo "${GREEN}OK${END}" && rm t3_*} || echo "${RED}KO${END}"

#infile pb tests
echo "${ITA}Tests infile pb${END}"
touch infile_r infile_no_r && chmod u-r infile_no_r

echo -n "Test 1 : ./pipex infile_r touch truc touch truc2 t1_output \t--> "
./pipex infile_r "touch truc" "touch truc2" t1_output
[[ $(ls -l | egrep "truc2?" | wc -l) -eq 2 ]] && {echo "${GREEN}OK${END}" ; rm t1_* truc*} || echo "${RED}KO${END}"

echo -n "Test 2 : ./pipex infile_no_r touch NON touch OUI t2_output \t--> "
./pipex infile_no_r "touch NOT" "touch OUI" t2_output 2> stderr.txt
[[ !$(ls -l | grep "NOT") && $(ls -l | grep "OUI") ]] && {echo -n "${GREEN}OK${END}" ; rm t2_* OUI} || echo -n "${RED}KO${END}"
[[ $(cat stderr.txt | grep -i "permission denied") ]] && echo "${GREEN} (+ err msg ok)${END}" || echo "${YEL} (- err msg without \"Permission denied\")${END}"
rm stderr.txt infile*

#outfile pb tests
echo "${ITA}Tests outfile pb${END}"
touch outfile_w outfile_no_w && chmod u-w outfile_no_w

echo -n "Test 1 : ./pipex Makefile touch truc touch truc2 outfile_w \t--> "
./pipex Makefile "touch truc" "touch truc2" outfile_w
[[ $(ls -l | egrep "truc2?" | wc -l) -eq 2 ]] && {echo "${GREEN}OK${END}" ; rm -f truc*} || echo "${RED}KO${END}"

echo -n "Test 2 : ./pipex Makefile touch OUI touch NON outfile_no_w \t--> "
./pipex Makefile "touch OUI" "touch NON" outfile_no_w 2> stderr.txt
code=$(echo $?)
[[ !$(ls -l | grep "NOT") && $(ls -l | grep "OUI") ]] && {echo -n "${GREEN}OK${END}" ; rm OUI} || echo -n "${RED}KO${END}"
[[ $(cat stderr.txt | grep -i "permission denied") ]] && echo -n "${GREEN} (+ err msg ok)${END}" || echo -n "${YEL} (- err msg without \"Permission denied\")${END}"
[[ $code -eq 1 ]] && echo "${GREEN}(+ return status == 1 ok)${END}" || echo "${YEL}(- return status != 1 ko)${END}"
rm -f stderr.txt outfile*

#silmultaneous cmd test
echo "${ITA}Tests cmd silmutaneous${END}"

echo -n "Test 1 : ./pipex Makefile yes "echo yo" outf \t--> "
./pipex Makefile yes "echo yo" outf
[[ $(cat outf) -eq "yo" ]] && {echo "${GREEN}OK${END}" ; rm -f outf} || echo "${RED}KO${END}"

echo -n "Test 2 : ./pipex Makefile "sleep 2" "sleep 1" outf \t--> "
t1=$(date +%s)
./pipex Makefile "sleep 2" "sleep 1" outf
t2=$(date +%s)
[[ $((t2 - t1)) -eq 2 ]] && {echo "${GREEN}OK${END}" ; rm -f outf} || echo "${RED}KO${END}"

#echo -n "Test 3 : ./pipex Makefile "yes" "cati" outf \t--> "

#cmd with absolute path
echo "${ITA}Tests cmd absolut path${END}"

echo -n "Test 1 : ./pipex Makefile /usr/bin/ls /usr/bin/cat t1_output \t--> "
touch t1_output t1_expected
/usr/bin/ls < Makefile | /usr/bin/cat > t1_expected
./pipex "Makefile" "/usr/bin/ls" "/usr/bin/cat" "t1_output" 
diff t1_expected t1_output && {echo "${GREEN}OK${END}" && rm t1_*} || echo "${RED}KO${END}"

#non existing cmd w/ abs. path
echo "${ITA}Tests no exist cmd absolut path${END}"

echo -n "Test 1 : ./pipex Makefile /usr/bin/lsoip /usr/bin/cati outf \t--> "
/usr/bin/lsoip < Makefile 2>/dev/null | /usr/bin/cati > t1_expected 2>/dev/null
./pipex "Makefile" "/usr/bin/lsoip" "/usr/bin/cati" "outf" 2> stderr.txt
[[ -s stderr.txt ]] && {echo -n "${GREEN}OK${END}" && rm -f outf} || echo -n "${RED}KO${END}"
[[ $(cat stderr.txt | egrep -i "(file|directory)") ]] && echo -n "${GREEN} (+ err msg ok)${END}" || echo -n "${YEL} (- err msg without \"No such file or directory\")${END}"
# + verif code erreur

#non existing cmd w/ rel. path










#executable


