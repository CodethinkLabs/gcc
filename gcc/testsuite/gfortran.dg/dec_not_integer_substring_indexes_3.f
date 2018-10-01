! { dg-do compile }
! { dg-options "-fdec-non-integer-index" }
!
! Test not integer substring indexes
!
        PROGRAM not_integer_substring_indexes
          CHARACTER*5 st/'Tests'/
          CHARACTER*4 st2
          REAL ir/1.0/
          REAL ir2/4.0/

          st2 = st(ir:4)
          st2 = st(1:ir2)
          st2 = st(1.0:4)
          st2 = st(1:4.0)
          st2 = st(1.5:4)
        END
