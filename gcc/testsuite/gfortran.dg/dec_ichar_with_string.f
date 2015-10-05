! { dg-do compile }
! { dg-options "-std=extra-legacy" }
!
! Test ICHAR and IACHAR with more than one character as argument
!
        PROGRAM ichar_more_than_one_character
          CHARACTER*4 st/'Test'/
          INTEGER i

          i = ICHAR(st)
          if (i.NE.84) STOP 1
          i = IACHAR(st)
          if (i.NE.84) STOP 2
          i = ICHAR('Test')
          if (i.NE.84) STOP 3
          i = IACHAR('Test')
          if (i.NE.84) STOP 4
        END
