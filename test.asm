.
.	JUST ANOTHER TEST FILE FOR 
.	EVERYTHING
.	IT ADDS TWO NUMBERS AND WRITES OUT RESULT
.	AS AN ASCII CHARACTER
.	AND APPENDS TWO UNDERSCORES AT THE END
.
TEST	START	0
MAIN	STL	RETADR
.
.	ADDS NUM1 AND NUM2 AND STORE IN CHAR
.
	LDA	NUM1
	ADD	NUM2
	STA	CHAR
.
.	WRITE RESULTS
.
	LDA	THREE
	STA	LENGTH
	LDA	CHAR
	STA	BUFFER
	JSUB	WRITE
.
.	WRITE UNDERSCORES
.
	LDA	TWO
	STA	LENGTH
	LDA	SPACE
	STA	BUFFER
	JSUB	WRITE
.
.	DUNNO, NULL ARE NEWLINES 
.	WHO WOULD HAVE GUESSED
.
	LDA	TWO
	STA	LENGTH
	LDA	ZERO
	STA	BUFFER
	JSUB	WRITE
	LDL	RETADR
	RSUB
.
.	DATA FOR MAIN
.
NUM1	WORD	30	FIRST NUMBER
NUM2	WORD	35	SECOND NUMBER
TWO	WORD	2	NUMBER OF UNDERSCORES
THREE	WORD	3	LENGHT OF A WORD
SPACE	BYTE	C'__'	
CHAR	RESB	3	AREA FOR RESULT OF ADDITION
RETADR	RESW	1	DUNNO, I COPIED THIS FROM EXAMPLE
LENGTH	RESW	1	
BUFFER	RESW	256
.
.	WRITE SUBROUTINE
.	COPIED FROM EXAMPLE
.
WRITE	LDX	ZERO
WLOOP	TD	OUTPUT
	JEQ	WLOOP
	LDCH	BUFFER,X
	WD	OUTPUT
	TIX	LENGTH
	JLT	WLOOP
	RSUB
.
.	DATA FOR WRITE
.
OUTPUT	BYTE	X'06'	USE DEV06
ZERO	WORD	0
	END	MAIN