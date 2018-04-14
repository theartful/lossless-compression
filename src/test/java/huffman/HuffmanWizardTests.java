package test.java.huffman;

import main.java.huffman.HuffmanWizard;
import org.junit.jupiter.api.Test;

import java.io.*;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.fail;

/**
 * Created by theartful on 4/14/18.
 */
public class HuffmanWizardTests {

    @Test
    void compressAndDecompressTest1() {
        compressAndDecompressFile("test.bmp", "test.bin", "test-output.bmp",
                16, 65536 * 4);
        Process p;
        String s;
        try {
            p = Runtime.getRuntime().exec("diff test.bmp test-output.bmp");
            BufferedReader br = new BufferedReader(
                    new InputStreamReader(p.getInputStream()));
            s = br.readLine();
            assertEquals(null, s);
            p.waitFor();
            p.destroy();
        } catch (Exception e) {
            fail("Exception");
        }
    }

    @Test
    void compressAndDecompressTest2() {
        compressAndDecompressFile("test2.bmp", "test2.bin", "test2-output.bmp",
                8, 65536);
        Process p;
        String s;
        try {
            p = Runtime.getRuntime().exec("diff test2.bmp test-output2.bmp");
            BufferedReader br = new BufferedReader(
                    new InputStreamReader(p.getInputStream()));
            s = br.readLine();
            assertEquals(null, s);
            p.waitFor();
            p.destroy();
        } catch (Exception e) {
            fail("Exception");
        }
    }

    void compressAndDecompressFile(String fileName, String compressedFileName, String decompressedFileName,
                                   int wordLength, int blockSize) {
        try {
            FileInputStream fis = new FileInputStream(fileName);
            FileOutputStream fos = new FileOutputStream(compressedFileName);
            HuffmanWizard.encode(fis, fos, wordLength, blockSize);

            fis.close();
            fos.close();

            fis = new FileInputStream(compressedFileName);
            fos = new FileOutputStream(decompressedFileName);
            HuffmanWizard.decodeFile(fis, fos, wordLength);

            fis.close();
            fos.close();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
            fail("Exception occured");
            return;
        } catch (IOException e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

}
