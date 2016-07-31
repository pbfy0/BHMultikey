cp MultiKeyboard_mod\bin\MultiKeyboard.swf installer\
cp MultiKeyboard\Release\MultiKeyboard.dll MultiKeyboard_ane\MultiKeyboard\META-INF\ANE\Windows-x86\
rm -r installer\MultiKeyboard
cp -r MultiKeyboard_ane\MultiKeyboard installer\

Compress-Archive -Path installer\* -DestinationPath "release\BHMultikey_$($args[0]).zip"