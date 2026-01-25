--  with pack; use pack;

with ada.text_io;

-- pack.ads:  
--  package pack is 
--    X: Integer := 1;
--  end pack;

procedure main is 
   --  type X is null record; err
   X: Integer := 2;

   package t is 
      X: Integer := 2;
   end t;

   procedure test is 
      package t is 
         package t is 
            X: Integer := 3;
         end t;
         X: Integer := 2;


         package f is
            Z: Integer := t.X;
         end f;
      end t;

      procedure test is
      begin
         ada.text_io.put_line(Integer'Image(1234)); 
      end test;

      procedure bar is 
      begin
         test;
      end bar;
   begin
      --  t.X := 3;
      --  ada.text_io.put_line(Integer'Image(t.f.Z));  - 3
      bar; -- out 1234
   end test;
   
begin
   --  test;
   test;
   --  ada.text_io.put_line(Integer'Image(t)); -- OK: output - 2
end main;

-- 