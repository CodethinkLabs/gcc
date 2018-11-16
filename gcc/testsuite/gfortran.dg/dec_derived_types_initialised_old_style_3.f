! { dg-do run }
! { dg-options "-std=legacy -fdec-old-init" }
!
! Test old style initializers in derived types
!
        PROGRAM spec_in_var
          TYPE STRUCT1
            INTEGER*4      ID       /8/
            INTEGER*4      TYPE     /5/
            INTEGER*8      DEFVAL   /0/
            CHARACTER*(5)  NAME     /'tests'/
            LOGICAL*1      NIL      /0/
          END TYPE STRUCT1

          TYPE (STRUCT1) SINST

          if(SINST%ID.NE.8) STOP 1
          if(SINST%TYPE.NE.5) STOP 2
          if(SINST%DEFVAL.NE.0) STOP 3
          if(SINST%NAME.NE.'tests') STOP 4
          if(SINST%NIL) STOP 5
        END
