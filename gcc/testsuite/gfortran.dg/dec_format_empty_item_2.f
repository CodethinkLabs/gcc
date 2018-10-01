! { dg-do compile }
! { dg-options "-fdec" }
!
! Test blank/empty format items in format string
!
        PROGRAM blank_format_items
          INTEGER A/0/

          OPEN(1, status="scratch")
          WRITE(1, 10) 100
          REWIND(1)
          READ(1, 10) A
          IF (a.NE.100) STOP 1
          PRINT 10, A
10        FORMAT( I5,)
        END
