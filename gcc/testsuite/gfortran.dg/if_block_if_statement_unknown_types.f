! { dg-do compile }
!
! Check for unknown types in if statements and if blocks
!
        PROGRAM unknown_types_if_statements_blocks
          REAL     :: ar1 = 0.0
          REAL     :: ar2 = 1.0
          LOGICAL  :: l1 = .false.
          INTEGER  :: i1 = 1

          ! This throws added BT_UNKNOWN warning in parse.c
          if (cos(ar1).NE.1) THEN
            STOP 1
          endif

          ! This throws added BT_UNKNOWN warning in match.c
          if (abs(ar2 - 1.0) > 1.0D-6) STOP 2

          ! The following if statements don't throw warnings
          if (l1) STOP 3
          if (i1.NE.1) STOP 4
          if ((i1 + i1).EQ.1) STOP 5
        END
