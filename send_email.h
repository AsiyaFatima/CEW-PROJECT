#ifndef SEND_EMAIL_H_INCLUDED
#define SEND_EMAIL_H_INCLUDED

typedef struct {
    char *smtp_server;
    int smtp_port;
    char *sender_email;
    char *recipient_email;
    char *username;
    char *password;
} EmailConfig;

void initEmailConfig(EmailConfig *email, const char *server, int port, const char *sender,
                     const char *recipient, const char *user, const char *pass);

int sendEmail(EmailConfig *email, const char *subject, const char *body, const char *attachment_file);


#endif // SEND_EMAIL_H_INCLUDED
