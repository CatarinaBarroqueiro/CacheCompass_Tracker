// material.module.ts

import { CUSTOM_ELEMENTS_SCHEMA, NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { MatCardModule } from '@angular/material/card';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatSelectModule } from '@angular/material/select';
import { MatOptionModule } from '@angular/material/core';

@NgModule({
  declarations: [],
  imports: [
    CommonModule,
    MatCardModule,
    MatSelectModule,
    MatOptionModule,
    MatFormFieldModule
  ],
  exports: [
    MatCardModule,
    MatSelectModule,
    MatOptionModule,
    MatFormFieldModule
  ],
  schemas: [CUSTOM_ELEMENTS_SCHEMA]
})
export class MaterialModule { }
