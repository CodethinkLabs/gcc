! { dg-do compile }
! { dg-options "-std=extra-legacy" }
!
! Test character declaration with mixed string length and array specification
!
        PROGRAM character_declaration
          CHARACTER ASPEC_SLENGTH*2 (5) /'01','02','03','04','05'/
          CHARACTER SLENGTH_ASPEC(5)*2 /'01','02','03','04','05'/
          if (ASPEC_SLENGTH(3).NE.SLENGTH_ASPEC(3)) STOP 1
        END
