# EX11

כתבו סקריפט המקבל כקלט נתיב לקובץ )או שם של קובץ במיקום הנוכחי, ניתן להניח שקיים ושאינו מכיל רווחים בשמו ומילה כלשהי )ניתן להניח שאינה ריקה(, קורא את הקובץ ומדפיס למסך את מספר המילים בשורות שמכילות את המילה הזו, כאשר מספר השורה מופיע בתחילת ההדפסה )עם רווח ביניהם(. אם המילה הזו לא מופיעה כלל בקובץ, אין צורך להדפיס דבר.

### example
file.txt :
```
This is a text file
It contains many words
This is an example for simple text file
```

### output :
```
./ex11.sh file.txt text

1 5
3 8
```

שימו לב, מילה שמכילה את המילה שנחפש ל א תחשב. בדוגמה הנ"ל – אם תוכנס כקלט המילה tex , אין להדפיס כלום.


# EX12
כתבו סקריפט המקבל כקלט נתיב לספריה )או שם של ספריה במיקום הנוכחי, ניתן
להניח שקיימת, שאינה ריקה ושאינה מכילה רווחים בשמה( ועובר על כל הקבצים והתיקיות בספריה הזו ומדפיס רק את השמות של הקבצים המסתיימים ב .txt ולאחר מכן " is a file " וכן את השמות של כל התיקיות ולאחר מכן " is a directory ". כל הדפסה צריכה להיות בשורה חדשה, כאשר בתחילת כל שורה יופיע מספר השורה )אינדקס המתחיל ב- 1 .) על כל הקבצים והתיקיות להיות מודפסים לפי הסדר הלקסיקוגרפי שלהם.

לדוגמא, נניח שבספריה dir קיימים הקבצים/תיקיות הבאים:
- ex.c
- ex2 )נניח תיקייה(
- file.txt
- boo.txt
- boo2.txt
- video )נניח תיקייה(

### output : 
```
 ./ex12.sh dir
1 boo.txt is a file
2 boo2.txt is a file
file.txt is a file3
4 ex2 is a directory
video is a directory5
```

# EX13
כתבו סקריפט המקבל כקלט נתיב לספריה )או שם של ספריה במיקום הנוכחי, ניתן
להניח שקיימת ושאינה ריקה( ושם של קובץ. הסקריפט מחפש קבצים בעלי שם זהה בשתי רמות ביחס לנתיב שהתקבל. כלומר, בנתיב שהתקבל ובספריות הנמצאות בנתיב שהתקבל. במידה ונמצא קובץ בעל שם זהה, יש להדפיס את תוכנו. שימו לב, ייתכן שקיים יותר מקובץ אחד כזה )על כל הדפסה להיות בשורה חדשה(. לצורך אחידות, נדרש לעבור על תוכן של כל ספריה בסדר לקסיקוגרפי. לדוגמא, אם קיים קובץ בשם " a " וספריה בשם "b" באותה ספריה, תחילה נבדוק האם "a" הוא הקובץ המבוקש ורק לאחר מכן נבדוק את תוכן הספרייה " b". לעומת זאת, אם קיים קובץ בשם "b" וספריה בשם "a" באותה ספריה, תחילה נבדוק את תוכן הספרייה "a" ורק לאחר מכן נבדוק האם " b " הוא הקובץ המבוקש .
ניתן להניח ששמות כל הקבצים והספריות בנתיב שהתקבל, הם ללא רווחים . לא ניתן להניח שהנתיב שהתקבל לא מכיל רווחים .

דוגמא- נניח שבספריה בה אנו נמצאים נמצאת הספריה ) dir13 מצורפת, על מנת להבין את הדוגמא יש לעיין בתיקיה עצמה ובתוכן הקבצים(, אשר נראית כך:
```
Dir13 >
 	Aaa
  	Hello.txt
  	World.txt
 o	World
  	Hello.txt
 o	Ppp
   	Hello.txt
   	World.txt
 	Bbb.txt
 	Hello.txt
 	Zzz.tzt
```

### output : 
```
./ex13.sh dir13 hello.txt
hello world inside aaa! 
hello world!
 hello world inside world!
```

# EX14
נתון קובץ בנק המכיל נתונים על כל הפעולות של הבנק. כל שורה בקובץ מייצגת פעולה אחת עם שם הלקוח, פעולה )הפקדה +, משיכה -(, סניף, יום ושעה.
ניתן להניח שהקובץ קיים, שאינו מכיל רווחים בשמו, וכי תוכנו חוקי.
דוגמא לקובץ כזה )מצ"ב עבורכם קובץ כזה בשם Bank-log :)

עליכם לכתוב סקריפט אשר מקבל כפרמטר שם של הלקוח וקובץ בנק )נתיב לקובץ או שם של קובץ במיקום הנוכחי( ומדפיס את כל השורות בקובץ בנק בהם מופיע שם הלקוח וגם סה"כ היתרה בחשבונו. למשל עבור HAIM נקבל:

```
Haim 2000 Jerusalem 1/1/1998 15:18
Haim 1800 Jerusalem 15/1/1998 16:11
Total balance : 38000
```

 (שימו לב: רשמנו את השם המלא בתוך גרשיים " ", כך שהסקריפט יוכל להתייחס אליו כאל מחרוזת אחת. שם יכול להכיל יותר משתי מילים. שם עיר לא יכלול רווחים. )

הערות בקובץ בנק : השמות אינם מכילים רווחים, קריאה ללא גרשיים
