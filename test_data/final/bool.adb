with Ada.Text_IO; use Ada.Text_IO;

    
procedure Bool2 is 
   
    function fooTrue return Boolean is
    begin
        Put_Line("fooTrue");
        return true;
    end fooTrue;

   function fooFalse return Boolean is
   begin
      Put_Line("fooFalse");
      return false;
   end fooFalse;

begin

      Put_Line("Testing 1:");
      if (false or fooTrue) then
         Put_Line("true");
      else
         Put_Line("false");
      end if;

      Put_Line("Testing 2:");
      if (true or fooTrue) then
         Put_Line("true");
      else
         Put_Line("false");
      end if;
      

      Put_Line("Testing 3:");
      if (true and fooTrue) then
         Put_Line("true");
      else
         Put_Line("false");
      end if;

      Put_Line("Testing 4:");
      if (false and fooTrue) then
         Put_Line("true");
      else
         Put_Line("false");
      end if;

end Bool2;