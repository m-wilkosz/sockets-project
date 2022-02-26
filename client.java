import java.io.*;
import java.net.*;
import java.nio.*;
import java.util.*;
import java.nio.file.*;

public class klient {

    public static String keyGen(String text, String key) {
        int n = text.length();

        for (int i = 0; key.length() != n; i++) key += key.charAt(i);

        return key;
    }

    public static String textEnc(String text, String key) {
        String encText = "";

        for (int i = 0; i < text.length(); i++) {
            int c = (text.charAt(i) + key.charAt(i)) % 26 + 'A';
            encText += (char)(c);
        }
        return encText;
    }

    public static void main(String[] args) {
        try {
            Scanner userInput = new Scanner(System.in);

            System.out.println("provide host address: ");
            String host_addr = userInput.nextLine();
            InetAddress addr = InetAddress.getByName(host_addr);

            System.out.println("provide port number: ");
            int port = userInput.nextInt();

            Socket socket = new Socket(addr, port);
            System.out.println("\nconnection established\n");

            userInput.nextLine();
            System.out.println("enter filename: ");
            String filename = userInput.nextLine();
            File f = new File(filename);
            long file_len = f.length();

            Path filePath = Paths.get(filename);
            String fileContent = Files.readString(filePath);
            String keyword = "QRHSTOZPKU";
            String key = keyGen(fileContent, keyword);

            String encText = textEnc(fileContent, key);
            Files.writeString(filePath, encText);
            System.out.println("\ntext encrypted\n");

            DataOutputStream dos = new DataOutputStream(socket.getOutputStream());
            DataInputStream dis = new DataInputStream(socket.getInputStream());

            System.out.println("enter pattern: ");
            String pattern = userInput.nextLine();

            long patt_len = pattern.length();
            byte[] b = new byte[4];
            b[0] = (byte)(patt_len>>>24);
            b[1] = (byte)(patt_len>>>16);
            b[2] = (byte)(patt_len>>>8);
            b[3] = (byte)(patt_len);
            dos.write(b);

            b = new byte[pattern.length()];
            b = pattern.getBytes();
            dos.write(b);

            b = new byte[4];
            b[0] = (byte)(file_len>>>24);
            b[1] = (byte)(file_len>>>16);
            b[2] = (byte)(file_len>>>8);
            b[3] = (byte)(file_len);
            dos.write(b);

            Scanner scan = new Scanner(f);
            String contents = scan.nextLine();
            b = new byte[contents.length()];
            b = contents.getBytes();
            dos.write(b);
            while(scan.hasNextLine()) {
                contents = scan.nextLine();
                b = new byte[contents.length()];
                b = contents.getBytes();
                dos.write(b);
            }
            scan.close();
            System.out.println("\ntext and pattern sent\n");

            System.out.println("answer from server: " + dis.readInt());

            dos.close();
            dis.close();
            socket.close();
            System.out.println("\nconnection closed");
        } catch (Exception e) {
            System.out.println("error\n");
        }
        System.out.println("client finished");
    }
}