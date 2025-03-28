# AudioComposer
Raw audio manipulation

# Collaborating on a Private Repository

## Steps to Push Changes

### 1. Accept the Invitation
If you have been added as a collaborator, accept the invitation:
- Go to your GitHub account.
- Navigate to **Settings > Repositories**.
- Accept the invitation under the **Collaborations** section.

### 2. Clone the Repository
Once you have access, clone the repository using one of the following methods:

#### Using SSH (Recommended):
```bash
git clone git@github.com:YOUR_USERNAME/REPO_NAME.git
```

#### Using HTTPS (For Private Repositories, Will Prompt for Username and Password):
```bash
git clone https://github.com/Raz-svg/AudioComposer.git  
```

When cloning a private repository using HTTPS, you will be prompted to enter your GitHub username and password (or a personal access token if two-factor authentication is enabled).

Replace `YOUR_USERNAME` with your GitHub username and `REPO_NAME` with the repository name.

### 3. Navigate to the Repository Directory
```bash
cd REPO_NAME
```

### 4. Create a New Branch (Optional but Recommended)
```bash
git checkout -b feature-branch-name
```

Replace `feature-branch-name` with a meaningful branch name.

### 5. Make Changes & Commit
Modify files as needed, then stage and commit changes:
```bash
git add .
git commit -m "Your commit message"
```

### 6. Push Changes to GitHub
Push your branch to the remote repository:
```bash
git push origin feature-branch-name
```

If using HTTPS, you may be prompted again for your GitHub credentials.

### 7. Create a Pull Request
- Go to the repository on GitHub.
- Navigate to the **Pull Requests** tab.
- Click **New Pull Request** and select your branch.
- Submit the pull request for review.

### 8. Merge (If Approved)
Once the pull request is reviewed and approved, it can be merged into the main branch.

---

## Troubleshooting

### Authentication Issues
If you get an authentication error, make sure you have:
- Configured SSH keys correctly (`~/.ssh/id_rsa.pub` should be added to your GitHub account).
- Logged into GitHub via CLI: `gh auth login`
- Used the correct repository URL.
- If using HTTPS, ensure you entered the correct username and password or personal access token.

### Permission Denied Error
If you get a **permission denied** error while pushing, confirm:
- You have been added as a collaborator.
- You have accepted the repository invitation.
- You have the correct SSH key added to your GitHub account (if using SSH).

---

## Additional Resources
- [GitHub SSH Key Setup](https://docs.github.com/en/authentication/connecting-to-github-with-ssh)
- [Git Basics](https://git-scm.com/book/en/v2)
- [GitHub Pull Requests](https://docs.github.com/en/github/collaborating-with-issues-and-pull-requests/about-pull-requests)


