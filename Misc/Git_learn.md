## Git

remote: HTTP Basic: Access denied fatal: Authentication failed for
出现这个错误需要配置一下 调用 
git config --system --unset credential.helper
这个首先是因为设置了证明，但是证书过期了导致的。需要使用 git config --system --unset credential.helper 表示不使用证书，他会让我们重新输入一次账户密码就可以了。
但是我因为是改了密码导致的，所以就算调用了也没有弹出窗口，还是出错，还好我用tortoise git里面有一个credential页，把里面的内容都删除掉，在pull就提示需要输入密码了。
而且，注意，这个不能使用git-bash,需要使用git-cmd的命令才行。
https://blog.csdn.net/zqian1994/article/details/82143111
