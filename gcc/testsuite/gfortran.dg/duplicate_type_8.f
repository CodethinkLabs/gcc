! { dg-do compile }

! PR fortran/30239
! Check for errors when a symbol gets declared a type twice, even if it
! is the same.
!

      INTEGER FUNCTION foo ()
      INTEGER :: x
      INTEGER :: x ! { dg-error "basic type of" }
      x = 42
      IF (X.NE.42) STOP 1
      END FUNCTION foo
