#!/bin/bash
S21_GREP="./s21_grep"

FILES=("file1.txt")

FLAGS=("e" "i" "v" "c" "l" "n" "h" "s" "f" "o" "iv" "in" "ln" "cn" "cv" "ch" "hs" "oc" "iv" "ic" "io" "ns" "nl" "ni" "hn" "hl")

success=0
fail=0

for file in "${FILES[@]}"; do
  echo "File: $file"

  for flag in "${FLAGS[@]}"; do
    echo "Flag: -$flag"

  
    if [[ $flag == "f" ]]; then
      patterns=("patterns.txt")
    else
      
      patterns=("quidem" "free" "and" "hello")
    fi

    for pattern in "${patterns[@]}"; do
      echo "Pattern: $pattern"
      
      echo "Using 'grep -$flag $pattern $file':"
      grep_output=$(grep "-$flag" "$pattern" "$file")
      echo "$grep_output"

     
      echo "Using '$S21_GREP -$flag $pattern $file':"
      s21_grep_output=$("$S21_GREP" "-$flag" "$pattern" "$file")
      echo "$s21_grep_output"

      if [ "$grep_output" = "$s21_grep_output" ]; then
        echo "Success"
        ((success++))
      else
        echo "Fail"
        ((fail++))
      fi

      echo "---------------------------"
    done
  done

  echo "==========================="
done

echo "Total successes: $success"
echo "Total failures: $fail"