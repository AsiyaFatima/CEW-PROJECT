#!/bin/bash

# Set email variables
recipient="wajeehaali267@gmail.com"
subject="Weather Updates"
body="Please find the attached weather report"

# Set file attachment path
attachment_path="temperature_result.txt"

# Set SMTP server and authentication details
smtp_server="smtp.gmail.com:587"
smtp_user="ahsanali0332.aa@gmail.com"
smtp_password="elitzmgweojlnvqr"

# Temporary file to store the email content
email_temp_file=$(mktemp /tmp/email_temp.XXXXXX)

# Construct the email content
echo -e "Subject: $subject\n\n$body" > "$email_temp_file"

# Attach the file using uuencode
echo "Attachment:" | uuencode "$attachment_path" "$(basename "$attachment_path")" >> "$email_temp_file"

# Send the email using ssmtp
ssmtp -au "$smtp_user" -ap "$smtp_password" "$recipient" < "$email_temp_file"

# Clean up temporary file
rm -f "$email_temp_file"

