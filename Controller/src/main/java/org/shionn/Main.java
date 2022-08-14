package org.shionn;

import java.io.BufferedWriter;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.net.Socket;

public class Main {

    public static void main(String[] args) {
        try (Socket socket = new Socket("192.168.50.200", 2300);
                BufferedWriter os = new BufferedWriter(new OutputStreamWriter(socket.getOutputStream()))) {
            os.write("abc");
            os.flush();
        } catch (Exception e) {
            e.printStackTrace();
        }
        System.out.println("end");
    }
}
