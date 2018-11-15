! { dg-do run }
! { dg-options "-std=legacy -fdec-duplicates" }

      PROGRAM TEST
      INTEGER :: X
      INTEGER :: X
      X = 42
      IF (X.NE.42) STOP 1
      END PROGRAM TEST
