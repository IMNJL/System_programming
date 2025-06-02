#!/usr/bin/awk -f
BEGIN {
print "Обработанные строки файла:";
total_files = 0;
total_lines = 0;
total_words = 0;
total_chars = 0;
# Используем find и записываем файлы в массив
command = "find //Users/pro/texts -type f -name '*.txt'"
while ((command | getline file) > 0) {
files[total_files++] = file;
}
close(command);
}
# Обрабатываем файлы из списка
{
total_lines++;
total_chars += length($0);
split($0, words, " ");
num_words = length(words);
total_words += num_words;
if (num_words > 0) {
index_to_modify = num_words - total_lines + 1;
if (index_to_modify < 1) index_to_modify = 1;
words[index_to_modify] = "BEG" words[index_to_modify];
}
print total_lines ":", $0, "=>", join(words);
}
# Функция объединения массива слов в строку
function join(arr, i, str) {
str = arr[1];
for (i = 2; i <= length(arr); i++) {
str = str " " arr[i];
}
return str;
}
END {
print "\nИТОГОВАЯ ИНФОРМАЦИЯ:";
print "Обработано файлов:", total_files;
print "Обработано строк:", total_lines;
print "Обработано слов:", total_words;
print "Обработано символов:", total_chars;
print "\nПользователи с 's' в имени:";
command = "cat /etc/passwd"
while ((command | getline line) > 0) {
split(line, fields, ":");
user = fields[1];
password = (fields[2] == "x") ? "есть" : "нет";
home = fields[6];
if (user ~ /s/) {

print "Пользователь:", user, "Пароль:", password, "Домашний каталог:", home;
}
}
close(command);
}
