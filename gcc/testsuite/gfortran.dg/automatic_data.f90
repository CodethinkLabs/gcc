! { dg-do compile }
! { dg-options "-std=legacy -fno-automatic" }
! An AUTOMATIC statement cannot be used with SAVE
FUNCTION X()
DATA y/2.5/
AUTOMATIC y ! { dg-error "DATA attribute conflicts with AUTOMATIC attribute" }
y = 1
END FUNCTION X
END
