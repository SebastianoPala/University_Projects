package com.unipi.PlayerHive.DTO.games;

import jakarta.validation.constraints.NotBlank;
import jakarta.validation.constraints.PastOrPresent;
import jakarta.validation.constraints.PositiveOrZero;
import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;

import java.time.LocalDate;
import java.util.List;

@NoArgsConstructor
@AllArgsConstructor
@Getter
@Setter

public class EditGameDTO {

    @NotBlank
    private String name;

    @PastOrPresent
    private LocalDate releaseDate;

    @PositiveOrZero
    private Double price;

    @PositiveOrZero
    private Integer discount;

    private String description;

    @NotBlank
    private String imageURL;

    private List<String> supportedOS;

    @PositiveOrZero
    private Integer achievements;

    private List<String> developers;

    private List<String> publishers;

    private List<String> genres;
}
