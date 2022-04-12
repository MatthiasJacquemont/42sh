#!/bin/sh



TESTS=(
    # ECHO
    "echo"
    "echo 'hi; echo hello'"
    "echo 'hi; echo' error"
    "echo -n 'hello there you dumb shmuck'"
    "echo -n 'hi'; echo hello"
    "echo -n           'hi'; echo        he    llo"
    "echo -n    \"       'hi'\"; echo        \"he    llo\""
    "echo -n -p -r Salut' dewad'"
    "echo ---p  dede \"$deede \"         -n -p "
    bash "echo -e-n Salut tu fe koi???\\ "
    "echo echo echo echo echo echo echo"
    "echo \"echo echo\" echo -e -n Doing tests"
    bash "echo \"hey\thow r u\ndoing?\""
    bash "echo -e \"hey\thow r u\ndoing?\""
    bash "echo -e -n \"hey\thow r u\ndoing?\""
    bash "echo -e '-n \"hey\thow r u\ndoing?\"'"

    #COMMAND WITH QUOTES
    "printf \"salut dede\""
    "printf 'salut dede'"
    "echo printf 'salut dede'"
    "ls \"-l -1\""

    # IF
    "if echo CONDITION;"
    "if echo CONDITION; fi;"
    "if echo CONDITION; then hehe; fi;"
    "if echo CONDITION; then echo THEN;"
    "if echo CONDITION; then echo THEN; fi"
    "if echo CONDITION; then echo THEN; else echo ELSE; fi"
    "if ERROR; then echo THEN; else echo ELSE; fi"
    "if ls; then echo THEN; else echo ELSE; fi"
    "if     ;then echo 1; else echo something; fi"
    "if echo; then echo salut; fi"
    "if echo 1; then if if echo salut; then echo dede; fi; then echo work; fi; fi"
    "if ;fi"
    "if if if if if"
    "if echo if; then; then"
    "if echo salut; then echo salut; else if echo salut; then else"
    "if [ 5 -eq 4 ]; then echo correct; elif echo elif; then echo hehe; fi"
    "if [ 5 -eq 5 ]; then echo correct; elif echo elif; then echo hehe; fi"
    "if [ 5 -eq 5 ]; then echo correct; then echo elif; then echo hehe; fi"
    "if echo 1; then if echo 2; then echo 3; fi; elif echo 4; then echo 5; else echo 6; fi"
    "if ! echo 1; then if echo 2; then echo 3; fi; elif echo 4; then echo 5; else echo 6; fi"
    "if echo 1; echo 2; echo 3; then echo 4; echo 5; else echo false; echo hoho; fi"
    "if echo 1; echo 2; ! echo 3; then echo 4; echo 5; else echo false; echo hoho; fi"
    "if printf 'salut la compagnie'; then echo c'est true; else echo c'est false; fi"
    "if printf 'salut la compagnie; then echo c'est true; else echo c'est false; fi"
    "if printf 'salut la compagnie'; then echo c'est true; echo tjrs true; echo encore true; else echo c'est false; fi"
    "if ls -1 -l; then ls; fi"
    "if ls; false; then ls; echo ls; echo list de files; else jamais call; fi"
    "if !ls; then jamais call; else printf 'tjrs call'; fi"
    "if !ls; else echo au cas ou; fi"
    "if ls; then ls; else fi; fi"
    "if false; true; then
echo a
echo b; echo c;
fi"
    "if false
true
then echo a;echo b; echo c; fi"

    #VARIABLES
    "echo \$i"
    "echo \$@"
    "echo \$*"
    "echo \$?"
    "echo \$\$"

    "cd ../..; echo \$OLDPWD"
    "cd ../src; echo \$PWD \$OLDPWD"
    "cd ../../; ls; echo \$OLDPWD"
    "cd ../; find -name lexer.c"

    "dede=12; printf 'La phrase est \$dede'"
    "dede=\$lala; lala=12; echo \$lala \$dede"
    "dede=\" \"; echo \$dede salut"
    #"dede=\$dede; echo \$dede; echo echo quand meme"
    "dede=\$matthias; matthias=\$jacquemont; jacquemont=trop; echo \$jacquemont echo \$dede"


    # NON BUILTIN COMMANDS
    "ls"
    "ls /bin"

    # WEEK2
    "for j in 1 2 3 4 5; do echo \"salut\"; done"
    #"while [ \$i -lt 5 ]; do echo hehe; i=\$((\$i + 1)); done"
    #"until [ \$i -lt 5 ]; do echo hehe; i=\$((\$i + 1)); done"
    #"until [ 0 -lt 5 ]; do echo hehe; i=\$((\$i + 1)); done"
    "! echo 1"
    "echo 1; ! echo 2"
    "! echo 1; echo 2"
    "! echo 1; ! echo 2"
    "! echo 1;! echo 2"
    "!echo 1; echo 2"
    "if ! echo COND; then echo THEN; else echo ELSE; fi"
    "if ! [ 3 -eq 4 ]; then echo THEN; else if echo IF; then echo THEN; fi; echo ELSE; fi"
    "if { echo 1 || echo 2 }; then { echo 3 || echo 4 }; fi"
    "if { echo 1 || echo 2; }; then { echo 3 || echo 4; }; fi"

    "echo \$i"
    "i=5; echo \\$i"
    #"i=0; while [ \\$i -lt 5 ]; do echo \\$i; i=9; done"
    #"i=0; until [ \\$i -lt 5 ]; do echo \\$i; i=9; done"
    #"i=0; until [ \\$i -lt 5 ]; do echo \\$i; i=9; done"

    #AS SCRIPT
    "tests"
    "demo.txt"
    "toto.txt"
    "test1.sh"


    #OPERATOR
    "echo toto || echo tata"
    "echo toto && echo tata"
    "echo toto || echo tata && echo hoho || echo haha"
    "echo toto && echo tata || echo hoho && echo haha"
    "(echo toto || echo tata) && (echo hoho && echo haha)"


    #WEEK 3
    "cd tests"
    "cd tests/test_files"
    "cd tata"
    "cd tests/tata"

    "if ! echo COND; then echo THEN; else echo ELSE; fi"
    "if ! [ 3 -eq 4 ]; then echo THEN; else if echo IF; then echo THEN; fi; echo ELSE; fi"
    "if { echo 1 || echo 2 }; then { echo 3 || echo 4 }; fi"
)


test_count=0
passed=0
failed=0

check()
{
    touch "actual result" "expected result" res3
    let "test_count+=1"

    if [ "${3}" == "no_error" ]; then
        exec 3>&2 2> /dev/null    # Disabling stderr
    fi

    # EXECUTION ##############################################
    if [ "${1}" == "from_file" ]; then
        ../builddir/42sh "${4}" > "actual result"
    else
         ../builddir/42sh -c "${1}" > "actual result"
    fi

    result=$?
    if [ "${1}" == "from_file" ]; then
        dash "${4}" > "expected result"
        expected=$?
    elif [ "${2}" == "dash" ]; then
        dash -c "${1}" > "expected result"
        expected=$?
    else
        bash -c "${1}" > "expected result"
        expected=$?
    fi
    # EXECUTION ##############################################


    if [ "${3}" == "no_error" ]; then
        exec 2>&3                 # Enabling stderr
    fi

    diff -u "actual result" "expected result" > res3
    dif=$?

    exit_value=0
    if [ "${1}" == "from_file" ]; then
        echo -e -n "\033[1m\033[4mTest $test_count :\033[0m ./builddir/42sh ${4} "
    else
        echo -e -n "\033[1m\033[4mTest $test_count :\033[0m ./builddir/42sh -c \"${1}\" "
    fi

    if [ $dif -eq 1 ] || [ $result -ne $expected ]; then
        echo -e "[\033[1m\033[31mKO\033[0m]"
        failed=$(($failed + 1))

        echo -e -n "\t\t- return value "
        if [ $result -ne $expected ]; then
            echo -e "[\033[1m\033[31mKO\033[0m]"
            echo -e "\t\t\tExpected \e[96m$expected\e[0m but got \e[96m$result\e[0m."
        else
            echo -e "[\e[36mOK\e[0m]"
        fi

        echo -e -n "\t\t- stdout "
        if [ $dif -eq 1 ]; then
            echo -e "[\033[1m\033[31mKO\033[0m]"
            echo -e -n "\033[96m"
            cat res3
            echo -e -n "\e[0m"
        else
            echo -e "[\e[36mOK\e[0m]"
        fi

        exit_value=1
    else
        echo -e "[\033[1m\033[32mOK\033[0m]"
        passed=$(($passed + 1))
    fi
    echo -e

    rm "actual result" "expected result" res3

    return $exit_value
}





echo -e "\n\e[7m################################ TESTS ###############################\e[0m\n"

path="$(find ~+ -type f -name 'shell_tests_c.sh')"
path="${path::-16}"
cd "$path"

error="nope"
mode="-c"

if [ "$#" -eq 1 ] && [ -e "$(find -name ${1})" ]; then
    if [ "${2}" == "no_error" ]; then
        error="no_error"
    fi

    echo -e "\033[1mFrom file : \033[0m\n"
    check "from_file" "dash" $error "$(find -name ${1})"
else

    if [ "${1}" == "no_error" ] || [ "${2}" == "no_error" ]; then
        error="no_error"
    fi

    for ((i = 0; i < ${#TESTS[@]}; i++))
    do
        res=0
        touch test_res
        if [ "${TESTS[$i]}" == "bash" ]; then
            i=$(($i + 1))
            if [ "${1}" == "print" ] || [ "${2}" == "print" ]; then
                check "${TESTS[$i]}" "bash" $error $mode;
            else
                check "${TESTS[$i]}" "bash" $error $mode > test_res;
            fi
            res=$?
        else
            if [ "${1}" == "print" ] || [ "${2}" == "print" ]; then
                check "${TESTS[$i]}" "dash" "$error";
            else
                check "${TESTS[$i]}" "dash" "$error" > test_res;
            fi
            res=$?
        fi

        if [ ! "${1}" == "print" ] && [ ! "${2}" == "print" ] && [ $res -ne 0 ]; then
            cat test_res
        fi
        rm test_res
    done
fi





echo -e "\n\e[7m############################# CONCLUSION #############################\e[0m\n"
echo -e "\tPassed :\033[1m\033[32m $passed\033[0m, Failed :\033[1m\033[31m $failed\033[0m, Tests : \033[1m$test_count\033[0m\n"

echo -e -n "\t\033[1mTotal : \033[36m$passed/$(($passed + $failed))\033[0m"
percent=$(($passed * 100 / ($passed + $failed)))
if [ $percent -lt 80 ]; then
    echo -e " (\033[1m\033[31m$percent\033[0m\033[1m%\033[0m)\n"
elif [ $percent -lt 100 ]; then
    echo -e " (\033[1m\033[93m$percent\033[0m\033[1m%\033[0m)\n"
else
    echo -e " (\033[1m\033[32m$percent\033[0m\033[1m%\033[0m)\n"
fi

echo -e "\e[93mUse \e[1m'./shell_tests_c.sh print'\e[21m to display successful tests too.\e[0m"
echo -e "\e[93mUse \e[1m'./shell_tests_c.sh no_error'\e[21m to hide stderr and leaks.\e[0m\n"
