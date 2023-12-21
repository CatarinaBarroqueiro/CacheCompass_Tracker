import { Component, ElementRef, OnInit, ViewChild } from '@angular/core';
import { MaterialModule } from '../../material/material.module';
import { HttpClient, HttpClientModule } from '@angular/common/http';
import { Router, RouterModule } from '@angular/router';
import { MatDialog } from '@angular/material/dialog';
import { DialogComponent } from '../../dialog/dialog.component';

@Component({
  selector: 'app-register',
  imports: [MaterialModule, HttpClientModule, RouterModule],
  templateUrl: './register.component.html',
  styleUrls: ['./register.component.css'],
  standalone:true
})

export class RegisterComponent implements OnInit {
  
  
  @ViewChild('regEmail') emailInput!: ElementRef<HTMLInputElement>;
  @ViewChild('regPass') passInput!: ElementRef<HTMLInputElement>;
  @ViewChild('regName') nameInput!: ElementRef<HTMLInputElement>;

  data: Array<{ email: string, forno: string, analiseLEMM: string, timeForno:string}> = [];

  constructor(
    private dialog: MatDialog,
    private http: HttpClient,
    private router: Router
  ) { }
  ngOnInit() {
    }


post() {
  // Get input values
  const name = this.nameInput.nativeElement.value;
  const email = this.emailInput.nativeElement.value;
  const password = this.passInput.nativeElement.value;

  // Validate inputs
  if (!name || !email || !password) {
    // Show a pop-up dialog for missing inputs
    this.openDialog('Please fill in all required fields.');
    return;
  }

  // Create user data object
  const userData = {
    name,
    email,
    flag: false,
    password,
    admin: false,
  };

  // Make HTTP request
  this.http.post<{ message: string }>('http://51.20.64.70:3000/user', userData).subscribe(
    (res) => {
      console.log('Successfully posted data:', res.message);
    },
    (error) => {
      console.error('Error posting data:', error);
      // Show a pop-up dialog for the error
      this.openDialog('Error posting data. Please try again later.');
    }
  );
}

openDialog(message: string): void {
  this.dialog.open(DialogComponent, {
    data: { message },
  });
}

}
