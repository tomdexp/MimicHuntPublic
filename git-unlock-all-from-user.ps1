# This is a powershell tool to unlock all git LFS files of an user in the repo


# Warn that this is a advanced (maybe destructive) operations and to only use it if the user know what its doing
Write-Output "WARNING : this is an advanced tool that can have destructive operations on the git repo, only use it if you know what you are doing"
$understood = Read-Host "Are you sure you are an advanced user ? (Y/N)"

if ($understood -ne "Y" -and $understood -ne "y") {
    Write-Output "Operation cancelled."
    Read-Host "Press Enter to exit."
    exit
}


# Prompt for the username to search for locks
$username = Read-Host "Enter the username to unlock files for"

# Get all locks as JSON
$locksJson = git lfs locks --json

# Check if the command was successful
if (!$?) {
    Write-Error "Failed to retrieve locks. Make sure you're in a Git repository and Git LFS is installed."
    Read-Host "Press Enter to exit."
    exit
}

# Convert JSON to objects
try {
    $locksObj = $locksJson | ConvertFrom-Json
} catch {
    Write-Error "Failed to parse JSON output from 'git lfs locks --json'."
    Read-Host "Press Enter to exit."
    exit
}

# Check if there are any locks
if (-not $locksObj -or $locksObj.Count -eq 0) {
    Write-Output "No locks found in the repository."
    Read-Host "Press Enter to exit."
    exit
}

# Filter locks for the specified user
$userLocks = $locksObj | Where-Object { $_.owner.name -eq $username }

$lockCount = $userLocks.Count

if ($lockCount -eq 0) {
    Write-Output "No locks found for user '$username'."
    Read-Host "Press Enter to exit."
    # Do not exit immediately; wait for user input
} else {
    Write-Output "Found $lockCount lock(s) for user '$username'."

    # Display the locked files
    $userLocks | ForEach-Object {
        Write-Output "Lock ID: $($_.id), File: $($_.path)"
    }

    # Ask for confirmation
    $confirm = Read-Host "Are you sure you want to unlock these files? (Y/N)"

    if ($confirm -ne "Y" -and $confirm -ne "y") {
        Write-Output "Operation cancelled."
        Read-Host "Press Enter to exit."
        exit
    }

    # Proceed to unlock
    foreach ($lock in $userLocks) {
        $lockId = $lock.id
        # Unlock each file by lock ID with --force
        git lfs unlock --id $lockId --force

        # Check if the unlock command was successful
        if ($?) {
            Write-Output "Unlocked file '$($lock.path)' with lock ID: $lockId"
        } else {
            Write-Error "Failed to unlock file '$($lock.path)' with lock ID: $lockId"
        }
    }

    Write-Output "All files for user '$username' have been processed."
    Read-Host "Press Enter to exit."
}
