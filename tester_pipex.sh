#! /usr/bin/zsh

# machine info for bin_path (at least ok for ls, ko for touch)
uname -s | grep -qi darwin && os=mac && bin_path=/bin
uname -s | grep -qi linux && os=linux && bin_path=/usr/bin

alias vlgppx='/usr/bin/valgrind --trace-children=yes --leak-check=full --track-fds=yes'

# print intro
echo "------------------------------------"
echo "------------------------------------"
echo "\tPIPEX TESTER"
echo "Started at $(date +%R) - $(date +"%d %B %Y")"
echo "by $USER on $os"
echo "bin_path : $bin_path" 
echo "------------------------------------"
echo "------------------------------------"

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

#makefile (dupliquer pour bonus? avec cat makefile | grep bonus avant !)
echo -n "Test makefile:"
make 1>/dev/null 2> stderrmake.txt
make > stdoutmakebis.txt
[[ -s stderrmake.txt ]] && echo "${RED} make a ecrit sur std err${END}" || echo -n "${GREEN} pas d'erreur make${END}" 
echo -n " -- "
cat stdoutmakebis.txt | egrep -viq "(nothin|already)" && echo "${RED}makefile relink?${END}" || echo "${GREEN}pas de relink${END}"
rm -rf stderrmake.txt stdoutmakebis.txt

#arg nb problem
echo -n "Test pb nombre arg (0, 1, 2 et 3): "
./pipex > no_arg.txt 2> no_arg_err.txt
./pipex Makefile > one_arg.txt 2> one_arg_err.txt
./pipex Makefile "echo yo" > two_arg.txt 2> two_arg_err.txt
./pipex Makefile "echo yo" "echo yi" > three_arg.txt 2> three_arg_err.txt
./pipex Makefile "echo yo" "echo yi" outfile > four_arg.txt 2> four_arg_err.txt
if [[ -s no_arg.txt && -s one_arg.txt && -s two_arg.txt && -s three_arg.txt ]] ;
then	echo -n "${YEL}pipex ecrit sur std out (fd 1)${END} "
fi
if [[ -s no_arg_err.txt || -s one_arg_err.txt || -s two_arg_err.txt || -s three_arg_err.txt ]] ;
then	echo -n "${GREEN}pipex ecrit sur std err (fd 2)${END} "
fi
if [[ $(cat no_arg.txt no_arg_err.txt one_arg.txt one_arg_err.txt two_arg.txt two_arg_err.txt three_arg.txt three_arg_err.txt | egrep "(yi|yo)") ]] ;
then	echo -n "${RED}pipex realise les commandes${END}"
fi
rm -rf *arg*.txt outfile

#basics tests
echo "\n${ITA}Test basics:${END}"

echo -n "Test 1 : ./pipex Makefile ls ls t1_output \t\t\t--> "
touch t1_output t1_expected #both created here bc of ls cmd
ls > t1_expected
./pipex "Makefile" "ls" "ls" "t1_output" 
code=$(echo $?)
diff t1_expected t1_output 2>/dev/null && {echo -n "${GREEN}OK${END}" && rm t1_*} || echo -n "${RED}KO${END}"
[[ $code -eq 0 ]] && echo " ${GREEN}(+ return status == 0 ok)${END}" || echo "${YEL}(- return status != 0 ko)${END}"

echo -n "Test 2 : ./pipex Makefile cat cat t2_output\t\t\t--> "
touch t2_output
./pipex "Makefile" "cat" "cat" "t2_output" 
code=$(echo $?)
diff Makefile t2_output 2>/dev/null && {echo -n "${GREEN}OK${END}" && rm t2_*} || echo -n "${RED}KO${END}"
[[ $code -eq 0 ]] && echo " ${GREEN}(+ return status == 0 ok)${END}" || echo "${YEL}(- return status != 0 ko)${END}"

echo -n "Test 3 : ./pipex Makefile yes head t3_output \t\t\t--> "
touch t3_output t3_expected
yes | head > t3_expected
./pipex "Makefile" "yes" "head" "t3_output" 
code=$(echo $?)
diff t3_expected t3_output 2>/dev/null && {echo -n "${GREEN}OK${END}" && rm t3_*} || echo -n "${RED}KO (cmd one after another instead of parallel)${END}"
[[ $code -eq 0 ]] && echo " ${GREEN}(+ return status == 0 ok)${END}" || echo " ${YEL}(- return status != 0 ko)${END}"

#cmd with absolute path
echo "${ITA}Tests cmd absolut path:${END}"

echo -n "Test 1 : ./pipex Makefile ${bin_path}/ls ${bin_path}/cat t1_output \t\t--> "
touch t1_output t1_expected
${bin_path}/ls < Makefile | ${bin_path}/cat > t1_expected
./pipex "Makefile" "${bin_path}/ls" "${bin_path}/cat" "t1_output" 
code=$(echo $?)
diff t1_expected t1_output 2>/dev/null && {echo -n "${GREEN}OK${END}" && rm t1_*} || echo -n "${RED}KO${END}"
[[ $code -eq 0 ]] && echo " ${GREEN}(+ return status == 0 ok)${END}" || echo " ${YEL}(- return status != 0 ko)${END}"

#non existing cmd w/ abs. path
echo "${ITA}Tests cmd not found absolut path:${END}"

echo -n "Test 1 : ./pipex Makefile ${bin_path}/lsoip ${bin_path}/cati outf \t\t--> "
${bin_path}/lsoip < Makefile 2>/dev/null | ${bin_path}/cati > outf 2>/dev/null
./pipex "Makefile" "${bin_path}/lsoip" "${bin_path}/cati" "outf" 2> stderr.txt
code=$(echo $?)
[[ -s stderr.txt ]] && {echo -n "${GREEN}OK${END}" && rm -f outf} || echo -n "${RED}KO${END}"
[[ $(cat stderr.txt | egrep -i "(file|directory)") ]] && echo -n "${GREEN} (+ err msg ok)${END}" || echo -n "${YEL} (- err msg without \"No such file or directory\")${END}"
[[ $code -eq 127 ]] && echo "${GREEN}(+ return status == 127 ok)${END}" || echo "${YEL}(- return status != 127 ko)${END}"
rm -f stderr.txt

echo -n "Test 2 : ./pipex Makefile touch OUI ${bin_path}/cati outf \t\t--> "
./pipex "Makefile" "touch OUI" "${bin_path}/cati" "outf" 2> stderr.txt
code=$(echo $?)
ls -l | grep -q OUI && {echo -n "${GREEN}OK${END}" && rm -f outf OUI} || echo -n "${RED}KO${END}"
[[ $(cat stderr.txt | egrep -i "(file|directory)") ]] && echo -n "${GREEN} (+ err msg ok)${END}" || echo -n "${YEL} (- err msg without \"No such file or directory\")${END}"
[[ $code -eq 127 ]] && echo "${GREEN}(+ return status == 127 ok)${END}" || echo "${YEL}(- return status != 127 ko)${END}"
rm -f stderr.txt

echo -n "Test 3 : ./pipex Makefile ${bin_path}/cati touch OUI outf \t\t--> "
./pipex "Makefile" "${bin_path}/cati" "touch OUI" "outf" 2> stderr.txt
code=$(echo $?)
ls -l | grep -q OUI && {echo -n "${GREEN}OK${END}" && rm -f outf OUI} || echo -n "${RED}KO${END}"
[[ $(cat stderr.txt | egrep -i "(file|directory)") ]] && echo -n "${GREEN} (+ err msg ok)${END}" || echo -n "${YEL} (- err msg without \"No such file or directory\")${END}"
[[ $code -eq 0 ]] && echo "${GREEN}(+ return status == 0 ok)${END}" || echo "${YEL}(- return status != 0 ko)${END}"
rm -f stderr.txt

#non existing cmd w/ rel. path
echo "${ITA}Tests cmd not found relative path:${END}"

echo -n "Test 1 : ./pipex Makefile lsoip cati outf \t\t\t--> "
lsoip < Makefile 2>/dev/null | cati > t1_expected 2>/dev/null
./pipex "Makefile" "lsoip" "cati" "outf" 2> stderr.txt
code=$(echo $?)
[[ -s stderr.txt ]] && {echo -n "${GREEN}OK${END}" && rm -f outf} || echo -n "${RED}KO${END}"
[[ $(cat stderr.txt | egrep -i "(command not found)") ]] && echo -n "${GREEN} (+ err msg ok)${END}" || echo -n "${YEL} (- err msg without \"Command not found\")${END}"
[[ $code -eq 127 ]] && echo "${GREEN}(+ return status == 127 ok)${END}" || echo "${YEL}(- return status != 127 ko)${END}"
rm stderr.txt t1_expected

echo -n "Test 2 : ./pipex Makefile touch OUI cati outf \t\t\t--> "
./pipex "Makefile" "touch OUI" "cati" "outf" 2> stderr.txt
code=$(echo $?)
ls -l | grep -q OUI && {echo -n "${GREEN}OK${END}" && rm -f outf OUI} || echo -n "${RED}KO${END}"
[[ $(cat stderr.txt | egrep -i "command not found") ]] && echo -n "${GREEN} (+ err msg ok)${END}" || echo -n "${YEL} (- err msg without \"No such file or directory\")${END}"
[[ $code -eq 127 ]] && echo "${GREEN}(+ return status == 127 ok)${END}" || echo "${YEL}(- return status != 127 ko)${END}"
rm -f stderr.txt

echo -n "Test 3 : ./pipex Makefile cati touch OUI outf \t\t\t--> "
./pipex "Makefile" "cati" "touch OUI" "outf" 2> stderr.txt
code=$(echo $?)
ls -l | grep -q OUI && {echo -n "${GREEN}OK${END}" && rm -f outf OUI} || echo -n "${RED}KO${END}"
[[ $(cat stderr.txt | egrep -i "command not found") ]] && echo -n "${GREEN} (+ err msg ok)${END}" || echo -n "${YEL} (- err msg without \"No such file or directory\")${END}"
[[ $code -eq 0 ]] && echo "${GREEN}(+ return status == 0 ok)${END}" || echo "${YEL}(- return status != 0 ko)${END}"
rm -f stderr.txt

#empty commands
echo "${ITA}Tests empty cmd:${END}"

echo -n "Test 1 : ./pipex Makefile \" \" \" \" outf \t\t\t\t--> "
./pipex Makefile " " " " outf 2> stderr.txt
code=$(echo $?)
[[ -s outf ]] || echo -n "${GREEN}OK${END}"
[[ -s outf ]] && echo -n "${RED}KO${END}"
[[ $(cat stderr.txt | egrep -ic "command not found") -eq 2 ]] && echo -n "${GREEN} (+ err msg ok)${END}" || echo -n "${YEL} (- err msg without \"Command not found\")${END}"
[[ $code -eq 127 ]] && echo "${GREEN}(+ return status == 127 ok)${END}" || echo "${YEL}(- return status != 127 ko)${END}"
rm -f outf stderr.txt

echo -n "Test 2 : ./pipex Makefile "touch OUI" \" \" outf \t\t\t--> "
./pipex Makefile "touch OUI" " " outf 2> stderr.txt
code=$(echo $?)
ls -l | grep -q OUI && {echo -n "${GREEN}OK${END}" && rm -f outf OUI} || echo -n "${RED}KO${END}"
cat stderr.txt | egrep -qi "command not found" && echo -n "${GREEN} (+ err msg ok)${END}" || echo -n "${YEL} (- err msg without \"Command not found\")${END}"
[[ $code -eq 127 ]] && echo "${GREEN}(+ return status == 127 ok)${END}" || echo "${YEL}(- return status != 127 ko)${END}"
rm -f outf stderr.txt

echo -n "Test 3 : ./pipex Makefile \" \" touch OUI outf \t\t\t--> "
./pipex Makefile "touch OUI" " " outf 2> stderr.txt
code=$(echo $?)
ls -l | grep -q OUI && {echo -n "${GREEN}OK${END}" && rm -f outf OUI} || echo -n "${RED}KO${END}"
cat stderr.txt | egrep -qi "command not found" && echo -n "${GREEN} (+ err msg ok)${END}" || echo -n "${YEL} (- err msg without \"Command not found\")${END}"
[[ $code -eq 127 ]] && echo "${GREEN}(+ return status == 127 ok)${END}" || echo "${YEL}(- return status != 127 ko)${END}"
rm -f outf stderr.txt

#infile pb tests
echo "${ITA}Tests infile pb:${END}"
touch infile_r infile_no_r && chmod u-r infile_no_r

echo -n "Test 1 : ./pipex infile_r touch truc touch truc2 t1_output\t--> "
./pipex infile_r "touch truc" "touch truc2" t1_output
code=$(echo $?)
[[ $(ls -l | egrep -c "truc2?") -eq 2 ]] && {echo -n "${GREEN}OK${END}" ; rm t1_* truc*} || echo -n "${RED}KO${END}"
[[ $code -eq 0 ]] && echo " ${GREEN}(+ return status == 0 ok)${END}" || echo " ${YEL}(- return status != 0 ko)${END}"

echo -n "Test 2 : ./pipex infile_no_r touch NON touch OUI t2_output\t--> "
./pipex infile_no_r "touch NOT" "touch OUI" t2_output 2> stderr.txt
code=$(echo $?)
[[ !$(ls -l | grep "NOT") && $(ls -l | grep "OUI") ]] && {echo -n "${GREEN}OK${END}" ; rm t2_* OUI} || echo -n "${RED}KO${END}"
[[ $(cat stderr.txt | grep -i "permission denied") ]] && echo -n " ${GREEN} (+ err msg ok)${END}" || echo -n " ${YEL} (- err msg without \"Permission denied\")${END}"
[[ $code -eq 0 ]] && echo " ${GREEN}(+ return status == 0 ok)${END}" || echo " ${YEL}(- return status != 0 ko)${END}"
rm stderr.txt

echo -n "Test 3 : ./pipex infile_no_r lsop echo yo t2_output\t\t--> "
./pipex infile_no_r lsop "echo yo" t2_output 2> stderr.txt
code=$(echo $?)
[[ $(cat t2_output) == "yo" ]] && {echo -n "${GREEN}OK${END}" ; rm t2_* } || echo -n "${RED}KO${END}"
[[ $(cat stderr.txt | grep -i "permission denied") ]] && echo -n " ${GREEN} (+ err msg ok)${END}" || echo -n " ${YEL} (- err msg without \"Permission denied\")${END}"
[[ $code -eq 0 ]] && echo " ${GREEN}(+ return status == 0 ok)${END}" || echo " ${YEL}(- return status != 0 ko)${END}"
rm stderr.txt infile*

#outfile pb tests
echo "${ITA}Tests outfile pb:${END}"
touch outfile_w outfile_no_w && chmod u-w outfile_no_w

echo -n "Test 1 : ./pipex Makefile touch truc touch truc2 outfile_w \t--> "
./pipex Makefile "touch truc" "touch truc2" outfile_w
code=$(echo $?)
[[ $(ls -l | egrep "truc2?" | wc -l) -eq 2 ]] && {echo -n "${GREEN}OK${END}" ; rm -f truc*} || echo -n "${RED}KO${END}"
[[ $code -eq 0 ]] && echo " ${GREEN}(+ return status == 0 ok)${END}" || echo " ${YEL}(- return status != 0 ko)${END}"

echo -n "Test 2 : ./pipex Makefile touch OUI touch NON outfile_no_w \t--> "
./pipex Makefile "touch OUI" "touch NON" outfile_no_w 2> stderr.txt
code=$(echo $?)
[[ !$(ls -l | grep "NOT") && $(ls -l | grep "OUI") ]] && {echo -n "${GREEN}OK${END}" ; rm OUI} || echo -n "${RED}KO${END}"
[[ $(cat stderr.txt | grep -i "permission denied") ]] && echo -n "${GREEN} (+ err msg ok)${END}" || echo -n "${YEL} (- err msg without \"Permission denied\")${END}"
[[ $code -eq 1 ]] && echo "${GREEN}(+ return status == 1 ok)${END}" || echo "${YEL}(- return status != 1 ko)${END}"
rm -f stderr.txt

echo -n "Test 3 : ./pipex Makefile touch OUI lsop outfile_no_w \t\t--> "
./pipex Makefile "touch OUI" lsop outfile_no_w 2> stderr.txt
code=$(echo $?)
[[ $(ls -l | grep "OUI") ]] && {echo -n "${GREEN}OK${END}" ; rm -f OUI } || echo -n "${RED}KO${END}"
[[ $(cat stderr.txt | grep -i "permission denied") ]] && echo -n "${GREEN} (+ err msg ok)${END}" || echo "${YEL} (- err msg without \"Permission denied\")${END}"
[[ $code -eq 1 ]] && echo "${GREEN}(+ return status == 1 ok)${END}" || echo "${YEL}(- return status != 1 ko)${END}"
rm -f stderr.txt outfile*

#silmultaneous cmd test
echo "${ITA}Tests silmutaneous cmd:${END}"

echo -n "Test 1 : ./pipex Makefile yes "echo yo" outf \t\t\t--> "
./pipex Makefile yes "echo yo" outf
code=$(echo $?)
[[ $(cat outf) -eq "yo" ]] && {echo -n "${GREEN}OK${END}" ; rm -f outf} || echo -n "${RED}KO${END}"
[[ $code -eq 0 ]] && echo " ${GREEN}(+ return status == 0 ok)${END}" || echo " ${YEL}(- return status != 0 ko)${END}"

echo -n "Test 2 : ./pipex Makefile "sleep 2" "sleep 1" outf \t\t\t--> "
t1=$(date +%s)
./pipex Makefile "sleep 2" "sleep 1" outf
code=$(echo $?)
t2=$(date +%s)
[[ $((t2 - t1)) -eq 2 ]] && {echo -n "${GREEN}OK${END}" ; rm -f outf} || echo -n "${RED}KO${END}"
[[ $code -eq 0 ]] && echo " ${GREEN}(+ return status == 0 ok)${END}" || echo " ${YEL}(- return status != 0 ko)${END}"

echo -n "Test 3 : ./pipex Makefile "yes" "cati" outf \t\t\t--> "
# si mal gere risque de faire planter le testeur ...
./pipex Makefile "yes" "cati" outf 2> stderr.txt
code=$(echo $?)
[[ -s outf ]] || echo -n "${GREEN}OK${END}"
[[ -s outf ]] && echo -n "${RED}KO${END}"
[[ $(cat stderr.txt | grep -i "command not found") ]] && echo -n "${GREEN} (+ err msg ok)${END}" || echo "${YEL} (- err msg without \"Command not found\")${END}"
[[ $code -eq 127 ]] && echo "${GREEN}(+ return status == 127 ok)${END}" || echo "${YEL}(- return status != 127 ko)${END}"
rm -f stderr.txt outf

# executable (+ pas les droits)
echo "${ITA}Tests with a exec:${END}"

echo "#include <stdio.h>\nint main(void){printf(\"yo\");}" > main.c && gcc main.c && gcc -o ls main.c && rm main.c

echo -n "Test 1 : ./pipex Makefile ./a.out cat outf \t\t\t--> "
./pipex Makefile ./a.out cat outf 2> stderr.txt
code=$(echo $?)
[[ $(cat outf) == "yo" ]] && {echo -n "${GREEN}OK${END}" ; rm -f outf} || echo "${RED}KO${END}"
[[ -s stderr.txt ]] && echo -n "${RED} KO : you wrote on stderr${END}"
[[ $code -eq 0 ]] && echo -n "${GREEN} (+ return status == 0 ok)${END}" || echo "${YEL}(- return status != 0 ko)${END}"
rm -f stderr.txt

echo -n "\nTest 2 : ./pipex Makefile ls ./ls outf \t\t\t\t--> "
./pipex Makefile ls ./ls outf 2> stderr.txt
code=$(echo $?)
[[ $(cat outf) == "yo" ]] && {echo -n "${GREEN}OK${END}" ; rm -f outf} || echo "${RED}KO${END}"
[[ -s stderr.txt ]] && echo -n "${RED} KO : you wrote on stderr${END}"
[[ $code -eq 0 ]] && echo -n " ${GREEN}(+ return status == 0 ok)${END}" || echo "${YEL}(- return status != 0 ko)${END}"
rm -f stderr.txt ls

echo -n "\nTest 3 : ./pipex Makefile ls ./a.out (chmod u-x) outf \t\t--> "
chmod u-x a.out
./pipex Makefile ls ./a.out outf 2> stderr.txt
code=$(echo $?)
[[ $code -eq 126 ]] && $(cat stderr.txt | grep -qi "permission denied") && {echo "${GREEN}OK (code 126 + permission denied)${END}" ; rm -f outf}
[[ $code -ne 126 ]] && echo -n "${YEL}KO (code != 126)${END}"
cat stderr.txt | grep -qi "permission denied" || echo -n "${YEL}KO (err msg != Permission denied)${END}"
rm -f stderr.txt a.out

# outfile created before executing ls
echo "${ITA}Tests outfile created before exec:${END}"

echo -n "Test 1 : ./pipex Makefile cat ls outf \t\t\t\t--> "
rm -f outf
./pipex Makefile cat ls outf 2>/dev/null
cat outf | grep -q "outf" && echo "${GREEN}OK${END}" || echo "${RED}KO (missing "outf" in ls result)${END}"
rm -f outf

echo -n "Test 2 : ./pipex Makefile ls cat outf \t\t\t\t--> "
rm -f outf
./pipex Makefile ls cat outf 2>/dev/null
cat outf | grep -q "outf" && echo "${GREEN}OK${END}" || echo "${RED}KO (missing "outf" in ls result)${END}"
rm -f outf

# env -i (with absolute cmd)
echo "${ITA}Tests with env -i:${END}"

echo -n "Test 1 : env -i ./pipex Makefile cat ls outf \t\t\t--> "
env -i ./pipex Makefile cat "echo yo" outf 2> stderr.txt
code=$(echo $?)
cat outf | grep -q yo && {echo -n "${GREEN}OK - WOW t'es chaud, explique moi stp!${END}" && [[ $code -eq 0 ]] && echo " ${GREEN}(return status == 0)${END}" || echo " ${YEL}(return status != 0)${END}"}
[[ $(cat stderr.txt | grep -ic "command not found") -eq 2 ]] && echo -n "${GREEN}OK${END}"
[[ $code -eq 127 ]] && echo " ${GREEN}(+ return status == 127 ok)${END}" || echo " ${YEL}(- return status != 127 ko)${END}"
rm -f outf stderr.txt

echo -n "Test 2 : env -i ./pipex Makefile ${bin_path}/cat ${bin_path}/cat outf\t--> "
env -i ./pipex Makefile ${bin_path}/cat ${bin_path}/cat outf 2> stderr.txt
code=$(echo $?)
diff Makefile outf 2>/dev/null && echo -n "${GREEN}OK${END}" || echo "${RED}KO${END}"
[[ $code -eq 0 ]] && echo " ${GREEN}(+ return status == 0 ok)${END}" || echo " ${YEL}(- return status != 0 ko)${END}"
rm -f outf stderr.txt

# unset $PATH (with absolute cmd)
echo "${ITA}Tests with unset \$PATH:${END}"
tmp_PATH=$PATH

echo -n "Test 1 : unset PATH && ./pipex Makefile cat ls outf \t\t\t--> "
unset PATH
./pipex Makefile ls cat outf 2> stderr.txt
code=$(echo $?)
PATH=$tmp_PATH && export PATH
[[ $(cat stderr.txt | grep -ic "command not found") -eq 2 ]] && echo -n "${GREEN}OK${END}"
[[ $(cat stderr.txt | wc -l) -ne 2 ]] && echo -n "$ ${YEL}(- not two lines written to stderr)${END}"
[[ $code -eq 127 ]] && echo " ${GREEN}(+ return status == 127 ok)${END}" || echo " ${YEL}(- return status != 127 ko)${END}"
rm -f outf stderr.txt

echo -n "Test 2 : unset PATH && ./pipex Makefile ${bin_path}/cat ${bin_path}/cat outf \t--> "
unset PATH
./pipex Makefile ${bin_path}/cat ${bin_path}/cat outf 2> stderr.txt
code=$(echo $?)
PATH=$tmp_PATH && export PATH
diff Makefile outf 2>/dev/null && echo -n "${GREEN}OK${END}" || echo "${RED}KO${END}"
[[ $code -eq 0 ]] && echo " ${GREEN}(+ return status == 0 ok)${END}" || echo " ${YEL}(- return status != 0 ko)${END}"
rm -f outf stderr.txt

# valgrind
echo "${ITA}Tests with valgrind:${END}"

echo -n "Test 1 : vlgppx ./pipex Makefile cat cat outf \t\t\t--> "
vlgppx ./pipex Makefile cat cat outf 2> vlg.txt

first_proc=$(cat vlg.txt | grep -m1 -A 1 "HEAP SUMMARY" | tail -n1 | grep -o "[0-9]* bytes" | cut -d' ' -f1)
second_proc=$(cat vlg.txt | grep -m2 -A 1 "HEAP SUMMARY" | tail -n1 | grep -o "[0-9]* bytes" | cut -d' ' -f1)
main_proc=$(cat vlg.txt | grep -m3 -A 1 "HEAP SUMMARY" | tail -n1 | grep -o "[0-9]* bytes" | cut -d' ' -f1)
[[ $first_proc -eq 0 ]] && echo -n "${GREEN}no leak first proc${END}" || echo -n "${RED}$first_proc leaks first proc${END}"
[[ $second_proc -eq 0 ]] && echo -n "${GREEN} - no leak second proc${END}" || echo -n "${RED} - $second_proc leaks second proc${END}"
[[ $main_proc -eq 0 ]] && echo "${GREEN} - no leak main proc${END}" || echo "${RED} - $main_proc leaks main proc${END}"
rm -f outf vlg.txt

echo -n "Test 2 : vlgppx ./pipex Makefile yes head outf \t\t\t--> "
vlgppx ./pipex Makefile yes head outf 2> vlg.txt

first_proc=$(cat vlg.txt | grep -m1 -A 1 "HEAP SUMMARY" | tail -n1 | egrep -o "[0-9]*,?[0-9]* bytes" | cut -d' ' -f1)
second_proc=$(cat vlg.txt | grep -m2 -A 1 "HEAP SUMMARY" | tail -n1 | egrep -o "[0-9]*,?[0-9]* bytes" | cut -d' ' -f1)
main_proc=$(cat vlg.txt | grep -m3 -A 1 "HEAP SUMMARY" | tail -n1 | egrep -o "[0-9]*,?[0-9]* bytes" | cut -d' ' -f1)
echo -n "${GREEN}$first_proc leaks first proc (it's ok)${END}"
[[ $second_proc -eq 0 ]] && echo -n "${GREEN} - no leak second proc${END}" || echo -n "${RED} - $second_proc leaks second proc${END}"
[[ $main_proc -eq 0 ]] && echo "${GREEN} - no leak main proc${END}" || echo "${RED} - $main_proc leaks main proc${END}"
rm -f outf vlg.txt

# bonus
	# multi comd
	# here doc + append
# valgrind
	# leaks
	# segfault
	# opened fd

make fclean >/dev/null 2>&1
