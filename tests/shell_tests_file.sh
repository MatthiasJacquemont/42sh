#!/bin/sh

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
    ../builddir/42sh "${1}" > "actual result"
    result=$?

    if [ "${2}" == "dash" ]; then
        dash "${1}" > "expected result"
        expected=$?
    else
        bash "${1}" > "expected result"
        expected=$?
    fi
    # EXECUTION ##############################################


    if [ "${3}" == "no_error" ]; then
        exec 2>&3                 # Enabling stderr
    fi

    diff -u "actual result" "expected result" > res3
    dif=$?

    exit_value=0
    echo -e -n "\033[1m\033[4mTest $test_count :\033[0m ./builddir/42sh $(basename ${1}) "

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

path="$(find ~+ -type f -name 'shell_tests_file.sh')"
path="${path::-19}"
cd "$path"

error="nope"
mode="-c"

if [ "${1}" == "no_error" ] || [ "${2}" == "no_error" ]; then
    error="no_error"
fi

files_path="$(find ~+ -type d -name 'files_test')/*"
#echo $files_path
for file in $files_path; do #echo "$file"; done
    res=0

    #file="$(find ~+ -type d -name files_test)/${file:2}"
    #echo $file

    touch test_res
    if [ "${1}" == "print" ] || [ "${2}" == "print" ]; then
        check "$file" "dash" "$error";
    else
        check "$file" "dash" "$error" > test_res;
        #echo hh
    fi
    res=$?

    if [ ! "${1}" == "print" ] && [ ! "${2}" == "print" ] && [ $res -ne 0 ]; then
        cat test_res
    fi
    rm test_res
done





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

echo -e "\e[93mUse \e[1m'./shell_tests_file.sh print'\e[21m to display successful tests too.\e[0m"
echo -e "\e[93mUse \e[1m'./shell_tests_file.sh no_error'\e[21m to hide stderr and leaks.\e[0m\n"
