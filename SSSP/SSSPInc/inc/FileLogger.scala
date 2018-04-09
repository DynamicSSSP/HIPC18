package inc

import java.io.{File, PrintWriter}


object FileLogger {
  var FILE : PrintWriter= null
  def open(logfile: String) : Unit = {
    FILE = new PrintWriter(new File(logfile));
  }
  def println(x: Any) : Unit ={
    FILE.println(x);
    FILE.flush();
  }
  def print(x:Any) : Unit = {
    FILE.print(x)
  }
  def error(x: Any) : Unit = {
    FILE.print("Error: ");
    FILE.println(x);
  }
  def close(): Unit = {
    FILE.close();
  }

}
