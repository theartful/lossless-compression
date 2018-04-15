package main.java;


import main.java.lzw.LzwWizard;

import java.io.FileInputStream;
import java.io.FileOutputStream;

public class Main {

    public static void main(String[] args) {
        try {
            System.out.println("Encoder: ");
            FileInputStream fis = new FileInputStream("test.bin");
            FileOutputStream fos = new FileOutputStream("test.bin.bin");
            LzwWizard.encodeFile(fis, fos);

            fis.close();
            fos.close();
            /*
            System.out.println("Decoder: ");
            FileInputStream fis2 = new FileInputStream("test4.bin");
            FileOutputStream fos2 = new FileOutputStream("test4-out.txt");
            LzwWizard.decodeFile(fis2, fos2);
            fis2.close();
            fos2.close();
            */
        } catch (Exception e) {
            System.out.println("EXCEPTION!!");
            e.printStackTrace();
        }
    }
}
