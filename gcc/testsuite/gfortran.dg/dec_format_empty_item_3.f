! { dg-do compile }
! { dg-options "-fdec -fno-dec-blank-format-item" }
!
! Test blank/empty format items in format string
!
        PROGRAM blank_format_items
          INTEGER A/0/

          OPEN(1, status="scratch")
          WRITE(1, 10) 100 ! { dg-error "FORMAT label 10 at \\(1\\) not defined" }
          REWIND(1)
          READ(1, 10) A ! { dg-error "FORMAT label 10 at \\(1\\) not defined" }
          IF (a.NE.100) STOP 1
          PRINT 10, A ! { dg-error "FORMAT label 10 at \\(1\\) not defined" }
10        FORMAT( I5,) ! { dg-error "Unexpected element" }
        END
