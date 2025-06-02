BEGIN {
    file_count = 1
    line_count = 0
    cnt = 0
    cnt1 = 0
    printf "%-30s %-15s %-30s\n", "Имя пользователя", "Пароль", "Домашний каталог"
    print "---------------------------------------------------------------"
}

$1 ~ /s/ { 

    if ($2 == "*" || $2 == "") {
        password_status = "Нет"
	cnt++
    } else {
        password_status = "Есть"
	cnt1++
    }

    split($0, fields, ":")
    printf "%-30s %-15s %-30s\n", fields[1], password_status, fields[6]

    line_count++
}

END {
    print "---------------------------------------------------------------"
    print "файлов: " file_count
    print "с буквой s пользователей " cnt1
    print "всего пользователей: " line_count
}
