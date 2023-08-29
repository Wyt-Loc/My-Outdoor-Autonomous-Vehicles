package com.sockerwindows;

import javax.swing.*;
import javax.swing.filechooser.FileSystemView;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.*;
import java.net.Socket;

class send {
    public void a() throws IOException {

        FileSystemView fsv = FileSystemView.getFileSystemView();

        JFileChooser fileChooser = new JFileChooser();
        fileChooser.setCurrentDirectory(fsv.getHomeDirectory());
        fileChooser.setDialogTitle("请选择要上传的文件...");
        fileChooser.setApproveButtonText("确定");
        fileChooser.setFileSelectionMode(JFileChooser.FILES_ONLY);

        int result = fileChooser.showOpenDialog(null);

        String path = null;
        if (JFileChooser.APPROVE_OPTION == result) {
            path = fileChooser.getSelectedFile().getPath();
            System.out.println("path: " + path);
        }

        System.out.println("得到了路径开始传输");
        //创建输入流
        BufferedInputStream bis = new BufferedInputStream(new FileInputStream(path));
        //创建Socket
        Socket client = new Socket("8.130.115.65", 8888);
        //输出流
        BufferedOutputStream bos = new BufferedOutputStream(client.getOutputStream());
        //写出数据
        byte[] b = new byte[1024 * 8];
        int len;
        while ((len = bis.read(b)) != -1) {
            bos.write(b, 0, len);
            bos.flush();
        }
        System.out.println("文件已上传");

        //关闭资源
        bos.close();
        client.close();
        bis.close();
        System.out.println("文件上传完成");
    }
}

public class win {
    public static void main(String args[]) {
        JFrame frm1 = new JFrame();
        frm1.setTitle("程序远程升级");
        frm1.setLayout(null);
        frm1.setBounds(400, 200, 800, 700);//设置窗口在屏幕上的位置和大小。距离屏幕左侧123个像素，上方345个像素，窗口宽666，高520
        frm1.setSize(800, 700);
        JButton bt1 = new JButton("选择文件");
        bt1.setBounds(200, 350, 120, 20);
        JButton bt2 = new JButton("弹出2");
        bt2.setBounds(600, 350, 120, 20);
        frm1.add(bt1);
        frm1.add(bt2);
        frm1.setVisible(true);
        frm1.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

        //弹出功能实现！！
        //方法前缀是bt1,意思是，点击了bt1后，弹出新的界面。
        bt1.addActionListener(new ActionListener() {
            //这里要看清楚啊， **bt1.addActionListener(new ActionListener()!)** **把光标移到感叹号处，按上回车键。根据系统提示添加重载方法。
            //然后我们直接在重载方法中写出要弹出的窗口的

            @Override
            public void actionPerformed(ActionEvent e) {
                JFrame frm2 = new JFrame();
                frm2.setLayout(null);
                frm2.setSize(460, 350);
                frm2.setTitle("请等待传输完成...");
                // 设置显示文本
                JLabel l3 = new JLabel("请选择想要上传的文件......");
                l3.setBounds(20, 100, 250, 30);

                // 文本显示
                frm2.add(l3);
                frm2.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
                frm2.setVisible(true);

                // 开始执行 代码逻辑
                System.out.println("开始");
                send s = new send();
                try {
                    s.a();
                    l3.setText("");
                    l3 = new JLabel("文件传输完成");
                    l3.setBounds(20, 100, 250, 30);
                    frm2.add(l3);
                } catch (IOException ex) {
                    ex.printStackTrace();
                    l3.setText("");
                    l3 = new JLabel("文件传输失败,请检查网络配置是否正确");
                    l3.setBounds(20, 100, 250, 30);
                    frm2.add(l3);
                }

                frm2.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
                frm2.setVisible(true);
            }
        });
    }
}
