! { dg-do compile }
! { dg-options "-fdec-override-kind -fno-dec-override-kind" }
!
! Test character declaration with mixed string length and array specification
!
        PROGRAM character_declaration
          CHARACTER ASPEC_SLENGTH*2 (5) /'01','02','03','04','05'/ ! { dg-error "Syntax error" }
          CHARACTER SLENGTH_ASPEC(5)*2 /'01','02','03','04','05'/
          if (ASPEC_SLENGTH(3).NE.SLENGTH_ASPEC(3)) STOP 1 ! { dg-error " Operands of comparison operator" }
        END
