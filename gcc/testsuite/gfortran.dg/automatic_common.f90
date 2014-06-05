! { dg-do compile }
! { dg-options "-std=legacy -fno-automatic" }
! A common variable may not have the AUTOMATIC attribute.
INTEGER, AUTOMATIC :: X
COMMON /COM/ X ! { dg-error "conflicts with AUTOMATIC attribute" }
END
