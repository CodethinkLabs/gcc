! { dg-do compile }
! { dg-options "-std=legacy -fno-automatic" }
! An AUTOMATIC statement cannot be used with SAVE
FUNCTION X()
REAL, SAVE, AUTOMATIC :: Y ! { dg-error "SAVE attribute conflicts with AUTOMATIC attribute" }
y = 1
END FUNCTION X
END
