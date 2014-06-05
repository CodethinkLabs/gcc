! { dg-do compile }
! { dg-options "-std=legacy -fno-automatic" }
! An AUTOMATIC statement cannot duplicated
FUNCTION X()
REAL, AUTOMATIC, AUTOMATIC :: Y ! { dg-error "Duplicate AUTOMATIC attribute" }
y = 1
END FUNCTION X
END
